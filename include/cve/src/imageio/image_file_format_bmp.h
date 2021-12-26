/* image_file_format_bmp.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_IMAGE_FILE_FORMAT_BMP_H_
#define CVE_IMAGE_FILE_FORMAT_BMP_H_

#include "cve/src/core/image.h"
#include "cve/src/imageio/image_file_format.h"

namespace cve
{
    struct BMP
    { };

    namespace bmp
    {
        /** Enumeration of compression types for BMP files. */
        enum class CompressionType : uint32
        {
            BI_RGB = 0,
            BI_RLE8 = 1,
            BI_RLE4 = 2,
            BI_BITFIELDS = 3,
            BI_JPEG = 4,
            BI_PNG = 5,
            BI_ALPHABITFIELDS = 6,
            BI_CMYK = 11,
            BI_CMYKRLE8 = 12,
            BI_CMYKRLE4 = 13
        };

        /** Enumeration of header types for BMP files.
          * See https://en.wikipedia.org/wiki/BMP_file_format#DIB_header_(bitmap_information_header) */
        enum class DIBHeaderType : uint32
        {
            BITMAPCOREHEADER = 14,
            BITMAPCOREHEADER2 = 60,
            OS22XBITMAPHEADER = 16,
            BITMAPINFOHEADER = 40,
            BITMAPV2INFOHEADER = 52,
            BITMAPV3INFOHEADER = 56,
            BITMAPV4HEADER = 108,
            BITMAPV5HEADER = 124
        };

        struct FileHeader
        {
            std::array<char, 2> id = {'B', 'M'};
            uint32 size = 0;
            uint32 dataOffset = 0;
        };

        struct DIBHeader
        {
            DIBHeaderType type = DIBHeaderType::BITMAPINFOHEADER;
            CompressionType compression = CompressionType::BI_RGB;
            uint32 width = 0;
            uint32 height = 0;
            uint16 colorPlanes = 1;
            uint16 bpp = 32;
            uint32 rawSize = 0;
            uint32 horizontalPixelPerMeter = 0;
            uint32 verticalPixelPerMeter = 0;
            uint32 colorTableSize = 0;
            uint32 colorTableChannels = 4;
            uint32 importantColors = 0;

        };

        template<typename ColorSpace>
        class ColorTable
        {
        public:
            using container = std::array<Pixel<ColorSpace>, 256>;
            using size_type = typename container::size_type;
            using value_type = typename container::value_type;
            using reference = typename container::reference;
            using const_reference = typename container::const_reference;

            ColorTable(const size_type n)
                :_size(n)
            { }

            size_type size() const
            {
                return _size;
            }

            reference operator[](const size_type idx)
            {
                return _data[idx];
            }

            const_reference operator[](const size_type idx) const
            {
                return _data[idx];
            }

        private:
            container _data = {};
            const size_type _size;
        };

    }

    /** Reads an image from a stream in BMP file format. */
    template<>
    class ImageReader<BMP>
    {
    public:
        template<typename ColorSpace>
        void operator()(std::istream &stream, Image<ColorSpace> &img) const
        {
            bmp::FileHeader fileHeader;
            if(!readFileHeader(stream, fileHeader))
                throw std::runtime_error("failed to read BMP header");

            bmp::DIBHeader dibHeader;
            if(!readDIBHeader(stream, dibHeader))
                throw std::runtime_error("failed to read DIB header");

            bmp::ColorTable<RGBA> colorTable(dibHeader.colorTableSize);
            if(!readColorTable(stream, dibHeader.colorTableChannels, colorTable))
                throw std::runtime_error("failed to read color table");

            img.resize(dibHeader.height, dibHeader.width);
            if(!readImage(stream, img, dibHeader, colorTable))
                throw std::runtime_error("failed to read color image data");
        }
    private:

        bool readFileHeader(std::istream &stream, bmp::FileHeader &header) const
        {
            stream.read(header.id.data(), header.id.size());
            if(static_cast<std::size_t>(stream.gcount()) != header.id.size())
                return false;

            if(header.id[0] != 'B' && header.id[1] != 'M')
                return false;

            if(!readUInt32(stream, header.size))
                return false;

            if(4 != skipData(stream, 4))
                return false;

            if(!readUInt32(stream, header.dataOffset))
                return false;

            return true;
        }

        bool readDIBHeader(std::istream &stream, bmp::DIBHeader &header) const
        {
            uint32 size;
            if(!readUInt32(stream, size))
                return false;
            header.type = static_cast<bmp::DIBHeaderType>(size);

            // legacy bitmapcore header is a special case
            if(header.type == bmp::DIBHeaderType::BITMAPCOREHEADER)
            {
                header.colorTableSize = 0;
                header.colorTableChannels = 3;

                uint16 width;
                uint16 height;
                if(!readUInt16(stream, width))
                    return false;
                header.width = width;
                if(!readUInt16(stream, height))
                    return false;
                header.height = height;
                if(!readUInt16(stream, header.colorPlanes))
                    return false;
                if(!readUInt16(stream, header.bpp))
                    return false;
            }
            else
            {
                header.colorTableChannels = 4;

                if(!readUInt32(stream, header.width))
                    return false;
                if(!readUInt32(stream, header.height))
                    return false;
                if(!readUInt16(stream, header.colorPlanes))
                    return false;
                if(!readUInt16(stream, header.bpp))
                    return false;

                uint32 compression;
                if(!readUInt32(stream, compression))
                    return false;
                header.compression = static_cast<bmp::CompressionType>(compression);

                if(!readUInt32(stream, header.rawSize))
                    return false;

                if(!readUInt32(stream, header.horizontalPixelPerMeter))
                    return false;

                if(!readUInt32(stream, header.verticalPixelPerMeter))
                    return false;

                if(!readUInt32(stream, header.colorTableSize))
                    return false;

                if(!readUInt32(stream, header.importantColors))
                    return false;

                if(header.type > bmp::DIBHeaderType::BITMAPINFOHEADER)
                {
                    const uint32 diff = size - static_cast<uint32>(bmp::DIBHeaderType::BITMAPINFOHEADER);
                    if(diff != skipData(stream, diff))
                        return false;
                }
            }

            // if the color table size was not set and bpp
            // determines color table lookups,then autocompute size
            if(header.colorTableSize == 0 && header.bpp < 16)
                header.colorTableSize = uint32{1} << header.bpp;

            return true;
        }

        bool readColorTable(std::istream &stream, const std::size_t channels, bmp::ColorTable<RGBA> &colorTable) const
        {
            std::array<char, 4> data;
            for(std::size_t i = 0; i < colorTable.size(); ++i)
            {
                stream.read(data.data(), channels);
                if(channels != static_cast<std::size_t>(stream.gcount()))
                    return false;

                if(channels == 4)
                {
                    colorTable[i][0] = data[3];
                    colorTable[i][1] = data[2];
                    colorTable[i][2] = data[1];
                    colorTable[i][3] = data[0];
                }
                else
                {
                    colorTable[i][0] = data[2];
                    colorTable[i][1] = data[1];
                    colorTable[i][2] = data[0];
                    colorTable[i][3] = 255;
                }
            }

            return true;
        }

        template<typename ColorSpace>
        bool readImage(std::istream &stream, Image<ColorSpace> &img, const bmp::DIBHeader &header, const bmp::ColorTable<RGBA> &colorTable) const
        {
            if(header.bpp == 32)
                return readImage32bpp(stream, img);
            else if(header.bpp == 24)
                return readImage24bpp(stream, img);
            else if(header.bpp == 16)
                return readImage16bpp(stream, img);
            else if(header.bpp == 1 || header.bpp == 2 || header.bpp == 4 || header.bpp == 8)
                return readImageColorTable(stream, img, colorTable, header.bpp);
            else
                throw std::runtime_error("unsupported bits per pixel");
        }

        template<typename ColorSpace>
        bool readImage32bpp(std::istream &stream, Image<ColorSpace> &img) const
        {
            std::array<char, 4> data;

            for(Index row = 0; row < img.rows(); ++row)
            {
                for(Index col = 0; col < img.cols(); ++col)
                {
                    stream.read(data.data(), data.size());
                    if(static_cast<std::size_t>(stream.gcount()) != data.size())
                        return false;

                    const auto pixel = Pixel<RGBA>(
                        static_cast<RGBA::ValueType>(data[3]),
                        static_cast<RGBA::ValueType>(data[2]),
                        static_cast<RGBA::ValueType>(data[1]),
                        static_cast<RGBA::ValueType>(data[0]));
                    img(img.rows() - row - 1, col) = pixel.convert<ColorSpace>();
                }
            }

            return true;
        }

        template<typename ColorSpace>
        bool readImage24bpp(std::istream &stream, Image<ColorSpace> &img) const
        {
            std::array<char, 3> data;
            const auto paddingBytes = (img.cols() * data.size()) % 4;

            for(Index row = 0; row < img.rows(); ++row)
            {
                for(Index col = 0; col < img.cols(); ++col)
                {
                    stream.read(data.data(), data.size());
                    if(static_cast<std::size_t>(stream.gcount()) != data.size())
                        return false;

                    const auto pixel = Pixel<RGB>(
                        static_cast<RGB::ValueType>(data[2]),
                        static_cast<RGB::ValueType>(data[1]),
                        static_cast<RGB::ValueType>(data[0]));
                    img(img.rows() - row - 1, col) = pixel.convert<ColorSpace>();
                }

                // skip padding bytes at the end of the row
                if(paddingBytes != skipData(stream, paddingBytes))
                    return false;
            }

            return true;
        }

        template<typename ColorSpace>
        bool readImage16bpp(std::istream &stream, Image<ColorSpace> &img) const
        {
            std::array<char, 2> data;
            const auto paddingBytes = (img.cols() * data.size()) % 4;

            for(Index row = 0; row < img.rows(); ++row)
            {
                for(Index col = 0; col < img.cols(); ++col)
                {
                    stream.read(data.data(), data.size());
                    if(static_cast<std::size_t>(stream.gcount()) != data.size())
                        return false;

                    const auto pixel = Pixel<RGBA>(
                        static_cast<RGBA::ValueType>(data[1] & 0x0f),
                        static_cast<RGBA::ValueType>(data[1] >> 4),
                        static_cast<RGBA::ValueType>(data[0] & 0x0f),
                        static_cast<RGBA::ValueType>(data[0] >> 4));
                    img(img.rows() - row - 1, col) = pixel.convert<ColorSpace>();
                }

                // skip padding bytes at the end of the row
                if(paddingBytes != skipData(stream, paddingBytes))
                    return false;
            }

            return true;
        }

        template<typename ColorSpace>
        bool readImageColorTable(std::istream &stream, Image<ColorSpace> &img, const bmp::ColorTable<RGBA> &colorTable, const std::size_t bpp) const
        {
            const auto paddingBytes = (img.cols() * bpp * 8) % 4;

            for(Index row = 0; row < img.rows(); ++row)
            {
                std::size_t bitcnt = 0;
                uint8 value = 0;

                for(Index col = 0; col < img.cols(); ++col)
                {
                    if(bitcnt % 8 == 0)
                    {
                        if(!stream.good())
                            return false;

                        value = static_cast<uint8>(stream.get());
                    }

                    const auto idx =  static_cast<std::size_t>(value >> (8 - bpp));
                    value = value << bpp;
                    bitcnt += bpp;

                    if(idx >= colorTable.size())
                        return false;

                    img(img.rows() - row - 1, col) = colorTable[idx].convert<ColorSpace>();
                }

                // skip padding bytes at the end of the row
                if(paddingBytes != skipData(stream, paddingBytes))
                    return false;
            }

            return true;
        }

        bool readUInt32(std::istream &stream, uint32 &value) const
        {
            std::array<char, 4> buf;
            stream.read(buf.data(), buf.size());
            value = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
            return static_cast<std::size_t>(stream.gcount()) == buf.size();
        }

        bool readUInt16(std::istream &stream, uint16 &value) const
        {
            std::array<char, 2> buf;
            stream.read(buf.data(), buf.size());
            value = buf[0] | (buf[1] << 8);
            return static_cast<std::size_t>(stream.gcount()) == buf.size();
        }

        std::size_t skipData(std::istream &stream, const std::size_t n) const
        {
            for(std::size_t i = 0; i < n; ++i)
            {
                if(!stream.good())
                    return i;

                stream.get();
            }

            return n;
        }
    };

    template<>
    class ImageWriter<BMP>
    {
    public:
        template<typename ColorSpace>
        void operator()(std::ostream &stream, const Image<ColorSpace> &img, const BMP&) const
        {
            if(!stream.good())
                throw std::runtime_error("stream is not good");

            bmp::FileHeader fileHeader;
            fileHeader.id[0] = 'B';
            fileHeader.id[1] = 'M';
            fileHeader.dataOffset = 12 + static_cast<uint32>(bmp::DIBHeaderType::BITMAPINFOHEADER);
            fileHeader.size = fileHeader.dataOffset + static_cast<uint32>(img.size() * 4);

            if(!writeFileHeader(stream, fileHeader))
                throw std::runtime_error("failed to write BMP header");

            bmp::DIBHeader dibHeader;
            dibHeader.type = bmp::DIBHeaderType::BITMAPINFOHEADER;
            dibHeader.colorPlanes = 1;
            dibHeader.colorTableSize = 0;
            dibHeader.compression = bmp::CompressionType::BI_RGB;
            dibHeader.width = img.cols();
            dibHeader.height = img.rows();
            dibHeader.horizontalPixelPerMeter = 1;
            dibHeader.verticalPixelPerMeter = 1;
            dibHeader.importantColors = 0;
            dibHeader.rawSize = 0;
            dibHeader.bpp = 24;

            if(!writeDIBHeader(stream, dibHeader))
                throw std::runtime_error("failed to write DIB header");

            if(!writeImage24bpp(stream, img))
                throw std::runtime_error("failed to write image data");
        }

    private:
        bool writeFileHeader(std::ostream &stream, const bmp::FileHeader &header) const
        {
            stream.write(header.id.data(), 2);
            return writeUInt32(stream, header.size) &&
                   writeUInt16(stream, 0) &&
                   writeUInt16(stream, 0) &&
                   writeUInt32(stream, header.dataOffset);
        }

        bool writeDIBHeader(std::ostream &stream, const bmp::DIBHeader &header) const
        {
            assert(bmp::DIBHeaderType::BITMAPINFOHEADER == header.type);
            assert(0 == header.colorTableSize);
            assert(1 == header.colorPlanes);
            assert(0 == header.importantColors);

            return writeUInt32(stream, static_cast<uint32>(header.type)) &&
                   writeUInt32(stream, header.width) &&
                   writeUInt32(stream, header.height) &&
                   writeUInt16(stream, header.colorPlanes) &&
                   writeUInt16(stream, header.bpp) &&
                   writeUInt32(stream, static_cast<uint32>(header.compression)) &&
                   writeUInt32(stream, header.rawSize) &&
                   writeUInt32(stream, header.horizontalPixelPerMeter) &&
                   writeUInt32(stream, header.verticalPixelPerMeter) &&
                   writeUInt32(stream, header.colorTableSize) &&
                   writeUInt32(stream, header.importantColors);
        }

        template<typename ColorSpace>
        bool writeImage32bpp(std::ostream &stream, const Image<ColorSpace> &img) const
        {
            std::array<char, 4> data;

            for(Index row = 0; row < img.rows(); ++row)
            {
                for(Index col = 0; col < img.cols(); ++col)
                {
                    const auto pixel = img(img.rows() - row - 1, col).template convert<RGBA>();
                    data[0] = static_cast<char>(pixel[3]);
                    data[1] = static_cast<char>(pixel[2]);
                    data[2] = static_cast<char>(pixel[1]);
                    data[3] = static_cast<char>(pixel[0]);

                    stream.write(data.data(), data.size());
                    if(!stream.good())
                        return false;
                }
            }

            return true;
        }

        template<typename ColorSpace>
        bool writeImage24bpp(std::ostream &stream, const Image<ColorSpace> &img) const
        {
            std::array<char, 3> data;
            const auto padding = (img.cols() * 3) % 4;

            for(Index row = 0; row < img.rows(); ++row)
            {
                for(Index col = 0; col < img.cols(); ++col)
                {
                    const auto pixel = img(img.rows() - row - 1, col).template convert<RGB>();
                    data[0] = static_cast<char>(pixel[2]);
                    data[1] = static_cast<char>(pixel[1]);
                    data[2] = static_cast<char>(pixel[0]);

                    stream.write(data.data(), data.size());
                    if(!stream.good())
                        return false;
                }

                for(Index i = 0; i < padding; ++i)
                    stream.put(0);

                if(!stream.good())
                    return false;
            }

            return true;
        }

        bool writeUInt32(std::ostream &stream, const uint32 value) const
        {
            std::array<char, 4> data;

            data[0] = static_cast<char>(value & 0x000000FF);
            data[1] = static_cast<char>((value >> 8) & 0x000000FF);
            data[2] = static_cast<char>((value >> 16) & 0x000000FF);
            data[3] = static_cast<char>((value >> 24) & 0x000000FF);

            stream.write(data.data(), data.size());
            if(!stream.good())
                return false;

            return true;
        }

        bool writeUInt16(std::ostream &stream, const uint16 value) const
        {
            std::array<char, 2> data;

            data[0] = static_cast<char>(value & 0x00FF);
            data[1] = static_cast<char>((value >> 8) & 0x00FF);

            stream.write(data.data(), data.size());
            if(!stream.good())
                return false;

            return true;
        }
    };

}

#endif
