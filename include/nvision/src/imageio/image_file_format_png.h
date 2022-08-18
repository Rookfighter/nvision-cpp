/* image_file_format_png.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef NVISION_IMAGE_FILE_FORMAT_PNG_H_
#define NVISION_IMAGE_FILE_FORMAT_PNG_H_

#include <png.h>
#include "nvision/src/core/image.h"
#include "nvision/src/imageio/image_file_format.h"

namespace nvision
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

    }

    struct PNG
    { };

    /** Reads an image from a stream in PNG file format. */
    template<>
    class ImageReader<PNG>
    {
    public:
        template<typename ColorSpace>
        void operator()(std::istream &stream, Image<ColorSpace> &img) const
        {
            // define the length of the PNG signature
            constexpr int PNGSIGLEN = 8;

            // allocate buffer for the file signature
            png_byte signature[PNGSIGLEN];
            // read 8 bytes into the signature buffer
            stream.read(reinterpret_cast<char*>(signature), PNGSIGLEN);

            // check if the read worked
            if(!stream.good())
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

            png_set_read_fn(pngPtr, static_cast<png_voidp>(&stream), png::__read);
            png_set_sig_bytes(pngPtr, PNGSIGLEN);

            png_read_info(pngPtr, infoPtr);

            // amount of bits per value
            png_uint_32 bitdepth = png_get_bit_depth(pngPtr, infoPtr);

            // if bit depth stream less than 8 then expand to 8 bits
            if(bitdepth < 8)
                png_set_packing(pngPtr);
            // libpng stores 16 bit values in big endian order
            // if this system stream little endian then swap the bytes
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

            auto rowPtrs = std::vector<png_bytep>(height);
            auto buf = std::vector<png_byte>(height * width * channels * bitdepth / 8);
            unsigned int stride = width * bitdepth * channels / 8;

            for (size_t i = 0; i < height; i++)
                rowPtrs[i] = buf.data() + i * stride;

            // read the png image into the buffer
            png_read_image(pngPtr, rowPtrs.data());
            png_read_end(pngPtr, NULL);

            img.resize(static_cast<Index>(height), static_cast<Index>(width));
            for(Index r = 0; r < height; ++r)
            {
                for(Index c = 0; c < width; ++c)
                {
                    Pixel<RGBA> rgba;
                    for(Index d = 0; d < channels; ++d)
                    {
                        Index i = r * width * channels + c * channels + d;
                        uint16 value = 0;

                        if(bitdepth == 8)
                            value = buf[i];
                        else if(bitdepth == 16)
                            value = reinterpret_cast<uint16_t*>(buf.data())[i];
                        else
                        {
                            png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
                            throw std::runtime_error("invalid bitdepth");
                        }

                        if(channels == 1)
                        {
                            rgba.setConstant(static_cast<uint8>(value));
                            rgba[3] = 255;
                        }
                        else if(d < rgba.size())
                        {
                            rgba[d] = static_cast<uint8>(value);
                        }
                    }

                    img(r, c) = rgba.convert<ColorSpace>();
                }
            }

            png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
        }
    };

    template<>
    class ImageWriter<PNG>
    {
    public:
        template<typename ColorSpace>
        void operator()(std::ostream &stream, const Image<ColorSpace> &img, const PNG &) const
        {
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
                png_destroy_write_struct(&pngPtr, &infoPtr);
                throw std::runtime_error("failed to write PNG data");
            }

            png_set_write_fn(pngPtr, static_cast<png_voidp>(&stream), png::__write, png::__flush);

            const png_uint_32 width =  img.cols();
            const png_uint_32 height = img.rows();
            const png_uint_32 channels = ColorSpace::Dimension;
            const png_uint_32 bitdepth = 8;
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

            // set basic information
            png_set_IHDR(pngPtr, infoPtr, width, height, bitdepth, colorType,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);

            // write info to file
            png_write_info(pngPtr, infoPtr);

            // allocate write buffer
            auto rowPtrs = std::vector<png_bytep>(height);
            auto buf = std::vector<png_byte>(height * width * channels * bitdepth / 8);
            const auto stride = width * channels * bitdepth / 8;

            for (size_t i = 0; i < height; i++)
                rowPtrs[i] = buf.data() + i * stride;

            for(Index r = 0; r < height; ++r)
            {
                for(Index c = 0; c < width; ++c)
                {
                    const auto rgba = img(r, c).template convert<RGBA>();
                    for(Index d = 0; d < channels; ++d)
                    {
                        Index i = r * width * channels + c * channels + d;
                        if(d < rgba.size())
                            buf[i] = static_cast<png_byte>(rgba[d]);
                        else
                            buf[i] = 255;
                    }
                }
            }

            png_write_image(pngPtr, rowPtrs.data());
            png_write_end(pngPtr, infoPtr);
            png_destroy_write_struct(&pngPtr, &infoPtr);
        }
    };

}

#endif
