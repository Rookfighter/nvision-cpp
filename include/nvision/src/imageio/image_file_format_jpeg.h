/* image_file_format_jpeg.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef NVISION_IMAGE_FILE_FORMAT_JPEG_H_
#define NVISION_IMAGE_FILE_FORMAT_JPEG_H_

#include <jpeglib.h>
#include "nvision/src/core/image.h"
#include "nvision/src/imageio/image_file_format.h"

namespace nvision
{
    namespace jpeg::internal
    {
        struct JPEGInputStream
        {
            jpeg_source_mgr pub;
            std::istream* stream;
            std::array<char, 4096> buffer;
        };

        inline void initInputStream(j_decompress_ptr cinfo)
        {
            auto src = reinterpret_cast<JPEGInputStream*>(cinfo->src);
            src->stream->seekg(0);
        }

        inline boolean fillInputStreamBuffer(j_decompress_ptr cinfo)
        {
            auto src = reinterpret_cast<JPEGInputStream*>(cinfo->src);

            if(!src->stream->good())
            {
                return FALSE;
            }

            src->stream->read(src->buffer.data(), src->buffer.size());
            src->pub.next_input_byte = reinterpret_cast<const JOCTET *>(src->buffer.data());
            src->pub.bytes_in_buffer = src->stream->gcount();

            return TRUE;
        }

        inline void skipInputBuffer(j_decompress_ptr cinfo, long count)
        {
            auto src = reinterpret_cast<JPEGInputStream*>(cinfo->src);
            if (count > 0)
            {
                while (count > static_cast<long>(src->pub.bytes_in_buffer))
                {
                  count -= static_cast<long>(src->pub.bytes_in_buffer);
                  // assume that fill buffer never returns false
                  (void)src->pub.fill_input_buffer(cinfo);
                }
                src->pub.next_input_byte += static_cast<size_t>(count);
                src->pub.bytes_in_buffer -= static_cast<size_t>(count);
            }
        }

        inline void terminateInputStream(j_decompress_ptr cinfo)
        {
            // Close the stream, can be nop
        }

        void makeInputStream(j_decompress_ptr cinfo, std::istream &in)
        {
            JPEGInputStream *src = nullptr;

            if (cinfo->src == nullptr)
            {
                cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(JPEGInputStream));
            }

            src = reinterpret_cast<JPEGInputStream*>(cinfo->src);
            src->pub.init_source = initInputStream;
            src->pub.fill_input_buffer = fillInputStreamBuffer;
            src->pub.skip_input_data = skipInputBuffer;
            src->pub.resync_to_restart = jpeg_resync_to_restart;
            src->pub.term_source = terminateInputStream;
            src->stream = &in;
            src->pub.bytes_in_buffer = 0;
            src->pub.next_input_byte = nullptr;
        }
    }

    struct JPEG
    { };

    template<>
    class ImageReader<JPEG>
    {
    public:
        template<typename Derived>
        void operator()(std::istream &stream, ImageBase<Derived> &img) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");

            using ColorSpace = typename ImageBase<Derived>::Scalar::ColorSpace;

            struct jpeg_decompress_struct cinfo;
            struct jpeg_error_mgr jerr;

            cinfo.err = jpeg_std_error(&jerr);

            jpeg_create_decompress(&cinfo);

            // create a stream data source
            jpeg::internal::makeInputStream(&cinfo, stream);

            if(jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK)
            {
                jpeg_destroy_decompress(&cinfo);
                throw std::runtime_error("failed to read JPEG header");
            }

            if(!jpeg_start_decompress(&cinfo))
            {
                jpeg_destroy_decompress(&cinfo);
                throw std::runtime_error("failed to start decompressing JPEG");
            }

            // after decompression the image dimensions are known
            img.derived().resize(cinfo.output_height, cinfo.output_width);

            // compute the stride and allocate a buffer for a single scan line
            auto row_stride = cinfo.output_width * cinfo.output_components;
            JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

            while (cinfo.output_scanline < cinfo.output_height)
            {
                const auto row = cinfo.output_scanline;
                const auto dim = jpeg_read_scanlines(&cinfo, buffer, 1);

                for(auto col = 0; col < cinfo.output_width; ++col)
                {
                    auto *start = buffer[0] + col * cinfo.output_components;
                    switch(cinfo.out_color_space)
                    {
                    case J_COLOR_SPACE::JCS_GRAYSCALE:
                        img(row, col) = pixel::convert<Gray, ColorSpace>(Pixel<Gray>{start[0]});
                        break;
                    case J_COLOR_SPACE::JCS_RGB:
                        img(row, col) = pixel::convert<RGB, ColorSpace>(Pixel<RGB>{start[0], start[1], start[2]});
                        break;
                    case J_COLOR_SPACE::JCS_EXT_RGBA:
                        img(row, col) = pixel::convert<RGBA, ColorSpace>(Pixel<RGBA>{start[0], start[1], start[2], start[3]});
                        break;
                    case J_COLOR_SPACE::JCS_EXT_ARGB:
                        img(row, col) = pixel::convert<RGBA, ColorSpace>(Pixel<RGBA>{start[1], start[2], start[3], start[0]});
                        break;

                    case J_COLOR_SPACE::JCS_EXT_BGR:
                        img(row, col) = pixel::convert<BGR, ColorSpace>(Pixel<BGR>{start[0], start[1], start[2]});
                        break;
                    case J_COLOR_SPACE::JCS_EXT_BGRA:
                        img(row, col) = pixel::convert<BGRA, ColorSpace>(Pixel<BGRA>{start[0], start[1], start[2], start[3]});
                        break;
                    case J_COLOR_SPACE::JCS_EXT_ABGR:
                        img(row, col) = pixel::convert<BGRA, ColorSpace>(Pixel<BGRA>{start[1], start[2], start[3], start[0]});
                        break;

                    case J_COLOR_SPACE::JCS_YCbCr:
                        img(row, col) = pixel::convert<YCbCr, ColorSpace>(Pixel<YCbCr>{start[0], start[1], start[2]});
                        break;

                    default:
                        throw std::runtime_error("unsupported JPEG colorspace");
                    }
                }
            }

            if(!jpeg_finish_decompress(&cinfo))
            {
                jpeg_destroy_decompress(&cinfo);
                throw std::runtime_error("failed to finish decompressing JPEG");
            }

            jpeg_destroy_decompress(&cinfo);
        }
    };

    template<>
    class ImageWriter<JPEG>
    {
    public:
        template<typename Derived>
        void operator()(std::ostream &stream, const ImageBase<Derived> &img, const JPEG &options) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
        }
    };
}

#endif
