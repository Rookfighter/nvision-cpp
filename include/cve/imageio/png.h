/* png.h
 *
 * Author: Fabian Meyer
 * Created On: 20 Jun 2019
 */

#ifndef CVE_PNG_H_
#define CVE_PNG_H_

#include <string>
#include <cctype>
#include <fstream>
#include <istream>
#include <ostream>
#include <unsupported/Eigen/CXX11/Tensor>
#include <png.h>
#include <iostream>

namespace cve
{
    namespace png
    {
        inline bool __isLittleEndian()
        {
            int n = 1;
            return (*(char *)&n == 1);
        }

        inline void __read(png_structp pngPtr,
            png_bytep data,
            png_size_t length)
        {
            // retrieve the stream object
            std::istream* is = reinterpret_cast<std::istream*>(png_get_io_ptr(pngPtr));
            is->read(reinterpret_cast<char*>(data), length);
        }

        inline void __write(png_structp pngPtr,
            png_bytep data,
            png_size_t length)
        {
            // retrieve the stream object
            std::ostream* os = reinterpret_cast<std::ostream*>(png_get_io_ptr(pngPtr));
            os->write(reinterpret_cast<char*>(data), length);
        }

        inline void __flush(png_structp pngPtr)
        {
            std::ostream* os = reinterpret_cast<std::ostream*>(png_get_io_ptr(pngPtr));
            os->flush();
        }

        /** Load an image from a stream in PNG file format.
          * @param is input stream
          * @param img output image */
        template<typename Scalar>
        inline void load(std::istream &is,
            Eigen::Tensor<Scalar, 3> &img)
        {
            // define the length of the PNG signature
            static const int PNGSIGLEN = 8;

            // allocate buffer for the file signature
            png_byte signature[PNGSIGLEN];
            // read 8 bytes into the signature buffer
            is.read(reinterpret_cast<char*>(signature), PNGSIGLEN);

            // check if the read worked
            if(!is.good())
                throw std::runtime_error("failed to read PNG signature");
            // check the signature
            if(png_sig_cmp(signature, 0, PNGSIGLEN) != 0)
                throw std::runtime_error("invalid PNG signature");

            png_bytep *rowPtrs = NULL;
            png_bytep buf = NULL;

            png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                NULL, NULL, NULL);
            if(!pngPtr)
                throw std::runtime_error("failed to allocate PNG read struct");
            png_infop infoPtr = png_create_info_struct(pngPtr);
            if(!infoPtr)
            {
                png_destroy_read_struct(&pngPtr, NULL, NULL);
                throw std::runtime_error("failed to allocate PNG info struct");
            }

            if(setjmp(png_jmpbuf(pngPtr)))
            {
                if(rowPtrs)
                    delete[] rowPtrs;
                if(buf)
                    delete[] buf;
                png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
                throw std::runtime_error("failed to read PNG data");
            }

            png_set_read_fn(pngPtr, static_cast<png_voidp>(&is), png::__read);
            png_set_sig_bytes(pngPtr, PNGSIGLEN);

            png_read_info(pngPtr, infoPtr);

            // amount of bits per value
            png_uint_32 bitdepth = png_get_bit_depth(pngPtr, infoPtr);

            // if bit depth is less than 8 then expand to 8 bits
            if(bitdepth < 8)
                png_set_packing(pngPtr);
            // libpng stores 16 bit values in big endian order
            // if this system is little endian then swap the bytes
            if (bitdepth == 16 && png::__isLittleEndian())
                png_set_swap(pngPtr);

            // update png info with settings from above
            png_read_update_info(pngPtr, infoPtr);

            // retrieve image dimensions
            png_uint_32 width =  png_get_image_width(pngPtr, infoPtr);
            png_uint_32 height = png_get_image_height(pngPtr, infoPtr);
            png_uint_32 channels = png_get_channels(pngPtr, infoPtr);
            // update bitdepth
            bitdepth = png_get_bit_depth(pngPtr, infoPtr);

            rowPtrs = new png_bytep[height];
            buf = new png_byte[height * width * channels * bitdepth / 8];
            unsigned int stride = width * bitdepth * channels / 8;

            for (size_t i = 0; i < height; i++)
                rowPtrs[i] = buf + i * stride;

            // read the png image into the buffer
            png_read_image(pngPtr, rowPtrs);
            png_read_end(pngPtr, NULL);

            img.resize(static_cast<Index>(height), static_cast<Index>(width), static_cast<Index>(channels));
            for(Index d = 0; d < img.dimension(2); ++d)
            {
                for(Index c = 0; c < img.dimension(1); ++c)
                {
                    for(Index r = 0; r < img.dimension(0); ++r)
                    {
                        Index i = r * width * channels + c * channels + d;
                        if(bitdepth == 8)
                            img(r, c, d) = static_cast<Scalar>(buf[i]);
                        else if(bitdepth == 16)
                            img(r, c, d) = static_cast<Scalar>(reinterpret_cast<uint16_t*>(buf)[i]);
                    }
                }
            }

            delete[] rowPtrs;
            delete[] buf;
            png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
        }

        /** Load an image from a file in PNG file format.
          * @param is input stream
          * @param img output image */
        template<typename Scalar>
        inline void load(const std::string &filename,
            Eigen::Tensor<Scalar, 3> &img)
        {
            std::ifstream is(filename);

            if(is.bad() || is.fail())
                throw std::runtime_error("failed to open " + filename);

            png::load(is, img);
        }


        /** Save an image to a stream in PNG file format.
          * @param os ouput stream
          * @param img input image */
        template<typename Scalar>
        inline void save(std::ostream &os,
            const Eigen::Tensor<Scalar, 3> &img)
        {
            png_bytep *rowPtrs = NULL;
            png_bytep buf = NULL;

            png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                NULL, NULL, NULL);
            if(!pngPtr)
                throw std::runtime_error("failed to allocate PNG write struct");
            png_infop infoPtr = png_create_info_struct(pngPtr);
            if(!infoPtr)
            {
                png_destroy_read_struct(&pngPtr, NULL, NULL);
                throw std::runtime_error("failed to allocate PNG info struct");
            }

            if(setjmp(png_jmpbuf(pngPtr)))
            {
                if(rowPtrs)
                    delete[] rowPtrs;
                if(buf)
                    delete[] buf;
                png_destroy_write_struct(&pngPtr, &infoPtr);
                throw std::runtime_error("failed to write PNG data");
            }

            png_set_write_fn(pngPtr, static_cast<png_voidp>(&os), png::__write, png::__flush);

            png_uint_32 width =  img.dimension(1);
            png_uint_32 height = img.dimension(0);
            png_uint_32 channels = img.dimension(2);
            png_uint_32 bitdepth = 8;
            png_uint_32 colorType;

            // determine color type depending on channels
            if(channels == 1)
                colorType = PNG_COLOR_TYPE_GRAY;
            else if(channels == 2)
                colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
            else if(channels == 3)
                colorType = PNG_COLOR_TYPE_RGB;
            else if(channels == 4)
                colorType = PNG_COLOR_TYPE_RGB_ALPHA;
            else
            {
                png_destroy_write_struct(&pngPtr, &infoPtr);
                throw std::runtime_error("too many image channels for PNG");
            }

            std::cout << "channels=" << channels << std::endl;

            // set basic information
            png_set_IHDR(pngPtr, infoPtr, width, height, bitdepth, colorType,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);

            // write info to file
            png_write_info(pngPtr, infoPtr);

            // allocate write buffer
            rowPtrs = new png_bytep[height];
            buf = new png_byte[height * width * channels * bitdepth / 8];
            unsigned int stride = width * bitdepth * channels / 8;

            for (size_t i = 0; i < height; i++)
                rowPtrs[i] = buf + i * stride;

            for(Index r = 0; r < img.dimension(0); ++r)
            {
                for(Index c = 0; c < img.dimension(1); ++c)
                {
                    for(Index d = 0; d < img.dimension(2); ++d)
                    {
                        Index i = r * width * channels + c * channels + d;
                        buf[i] = static_cast<png_byte>(img(r, c, d));
                    }
                }
            }

            png_write_image(pngPtr, rowPtrs);
            png_write_end(pngPtr, infoPtr);

            delete[] rowPtrs;
            delete[] buf;
            png_destroy_write_struct(&pngPtr, &infoPtr);
        }

        /** Save an image to a file in PNG file format.
          * @param os ouput stream
          * @param img input image */
        template<typename Scalar>
        inline void save(const std::string &filename,
            const Eigen::Tensor<Scalar, 3> &img)
        {
            std::ofstream os(filename);
            png::save(os, img);
        }

    }
}

#endif
