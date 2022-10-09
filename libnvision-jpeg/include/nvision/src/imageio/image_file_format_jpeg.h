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

#include <iostream>

namespace nvision
{
    namespace jpeg::internal
    {
        struct JPEGInputStream
        {
            static constexpr auto BufferSize = 4096;
            jpeg_source_mgr pub;
            std::istream* stream;
            std::array<char, BufferSize> buffer;
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

        inline void makeInputStream(j_decompress_ptr cinfo, std::istream &in)
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
    {
        float32 quality = 0.85F;
    };

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
            auto rowStride = cinfo.output_width * cinfo.output_components;
            auto buffer = std::vector<JOCTET>(rowStride);
            auto *bufferPtr = buffer.data();

            while (cinfo.output_scanline < cinfo.output_height)
            {
                const auto row = cinfo.output_scanline;
                const auto dim = jpeg_read_scanlines(&cinfo, &bufferPtr, 1);

                for(auto col = 0; col < cinfo.output_width; ++col)
                {
                    auto *start = bufferPtr + col * cinfo.output_components;
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

    namespace jpeg::internal
    {
        struct JPEGOutputStream
        {
            static constexpr auto BufferSize = 4096;
            jpeg_destination_mgr pub;
            std::ostream* stream;
            std::array<char, BufferSize> buffer;
        };

        inline void initOutputStream(j_compress_ptr cinfo)
        {
            auto *dest = reinterpret_cast<JPEGOutputStream*>(cinfo->dest);
            dest->pub.next_output_byte = reinterpret_cast<JOCTET *>(dest->buffer.data());
            dest->pub.free_in_buffer = dest->buffer.size();
        }

        inline boolean emptyOutputStream(j_compress_ptr cinfo)
        {
            auto *dest = reinterpret_cast<JPEGOutputStream*>(cinfo->dest);

            dest->stream->write(dest->buffer.data(), dest->buffer.size());
            dest->pub.next_output_byte = reinterpret_cast<JOCTET *>(dest->buffer.data());
            dest->pub.free_in_buffer = dest->buffer.size();

            return TRUE;
        }

        inline void terminateOutputStream(j_compress_ptr cinfo)
        {
            auto *dest = reinterpret_cast<JPEGOutputStream*>(cinfo->dest);
            dest->stream->flush();
        }

        inline void makeOutputStream(j_compress_ptr cinfo, std::ostream &os)
        {
            JPEGOutputStream *dest = nullptr;

            if (cinfo->dest == nullptr)
            {
                cinfo->dest = (struct jpeg_destination_mgr *) (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(JPEGOutputStream));
            }

            dest = reinterpret_cast<JPEGOutputStream*>(cinfo->dest);
            dest->pub.init_destination = initOutputStream;
            dest->pub.empty_output_buffer = emptyOutputStream;
            dest->pub.term_destination = terminateOutputStream;
            dest->stream = &os;
        }

        template<typename ColorSpace>
        inline J_COLOR_SPACE getColorSpace()
        {
            return J_COLOR_SPACE::JCS_UNKNOWN;
        }

        template<>
        inline J_COLOR_SPACE getColorSpace<Gray>()
        {
            return J_COLOR_SPACE::JCS_GRAYSCALE;
        }

        template<>
        inline J_COLOR_SPACE getColorSpace<RGB>()
        {
            return J_COLOR_SPACE::JCS_EXT_RGB;
        }

        template<>
        inline J_COLOR_SPACE getColorSpace<RGBA>()
        {
            return J_COLOR_SPACE::JCS_EXT_RGBA;
        }

        template<>
        inline J_COLOR_SPACE getColorSpace<BGR>()
        {
            return J_COLOR_SPACE::JCS_EXT_BGR;
        }

        template<>
        inline J_COLOR_SPACE getColorSpace<BGRA>()
        {
            return J_COLOR_SPACE::JCS_EXT_BGRA;
        }

        template<>
        inline J_COLOR_SPACE getColorSpace<YCbCr>()
        {
            return J_COLOR_SPACE::JCS_YCbCr;
        }
    }

    template<>
    class ImageWriter<JPEG>
    {
    public:
        template<typename Derived>
        void operator()(std::ostream &stream, const ImageBase<Derived> &img, const JPEG &options) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");

            using ColorSpace = typename ImageBase<Derived>::Scalar::ColorSpace;
            using IntegralColorSpace = typename GetIntegralColorSpace<ColorSpace>::type;

            const auto quality = static_cast<int>(options.quality * 100);
            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr jerr;

            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_compress(&cinfo);

            // create a stream data source
            jpeg::internal::makeOutputStream(&cinfo, stream);

            cinfo.image_width = img.cols();
            cinfo.image_height = img.rows();
            cinfo.input_components = IntegralColorSpace::Dimension;
            cinfo.in_color_space = jpeg::internal::getColorSpace<IntegralColorSpace>();

            assert(cinfo.in_color_space != J_COLOR_SPACE::JCS_UNKNOWN);
            std::cout << "color space " << cinfo.in_color_space << std::endl;
            std::cout << "comp " << cinfo.input_components << std::endl;

            jpeg_set_defaults(&cinfo);
            jpeg_set_quality(&cinfo, quality, TRUE);
            jpeg_start_compress(&cinfo, TRUE);

            // compute the stride and allocate a buffer for a single scan line
            const auto rowStride = img.cols() * IntegralColorSpace::Dimension;
            auto buffer = std::vector<unsigned char>(rowStride);
            auto *bufferPtr = buffer.data();

            while (cinfo.next_scanline < cinfo.image_height)
            {
                const auto row = cinfo.next_scanline;
                for(auto col = Index{0}; col < img.cols(); ++col)
                {
                    const auto integral = pixel::convert<ColorSpace, IntegralColorSpace>(img(row, col));
                    for(auto d = Index{0}; d < IntegralColorSpace::Dimension; ++d)
                    {
                        buffer[col * IntegralColorSpace::Dimension + d] = integral[d];
                    }
                }

                (void)jpeg_write_scanlines(&cinfo, &bufferPtr, 1);
            }

            jpeg_finish_compress(&cinfo);
            jpeg_destroy_compress(&cinfo);
        }
    };
}

#endif
