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

namespace cve
{
    namespace png
    {
        inline bool __isLittleEndian()
        {
            int n = 1;
            return (*(char *)&n == 1);
        }

        inline void __read(png_structp pngPtr, png_bytep data, png_size_t length)
        {
            // retrieve the stream object
            std::istream* is = static_cast<std::istream*>(png_get_io_ptr(pngPtr));
            is->read(reinterpret_cast<char*>(data), length);
        }

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
            if (bitdepth == 16 && __isLittleEndian())
                png_set_swap(pngPtr);

            // update png info with settings from above
            png_read_update_info(pngPtr, infoPtr);

            // retrieve image dimensions
            png_uint_32 width =  png_get_image_width(pngPtr, infoPtr);
            png_uint_32 height = png_get_image_height(pngPtr, infoPtr);
            png_uint_32 channels = png_get_channels(pngPtr, infoPtr);
            // update bitdepth
            bitdepth = png_get_bit_depth(pngPtr, infoPtr);

            png_bytep *rowPtrs = new png_bytep[height];
            char *buf = new char[height * width * channels * bitdepth / 8];
            unsigned int stride = width * bitdepth * channels / 8;

            for (size_t i = 0; i < height; i++)
                rowPtrs[i] = reinterpret_cast<png_bytep>(buf + i * stride);

            // read the png image into the buffer
            png_read_image(pngPtr, rowPtrs);

            img.resize(static_cast<Index>(height), static_cast<Index>(width), static_cast<Index>(channels));
            for(Index r = 0; r < img.dimension(0); ++r)
            {
                for(Index c = 0; c < img.dimension(1); ++c)
                {
                    for(Index d = 0; d < img.dimension(d); ++d)
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

        template<typename Scalar>
        inline void load(const std::string &filename,
            Eigen::Tensor<Scalar, 3> &img)
        {
            std::ifstream is(filename);

            if(is.bad() || is.fail())
                throw std::runtime_error("failed to open " + filename);

            png::load(is, img);
        }
    }
}

#endif
