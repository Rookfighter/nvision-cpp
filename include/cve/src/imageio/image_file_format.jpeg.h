/* image_file_format_jpeg.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_IMAGE_FILE_FORMAT_JPEG_H_
#define CVE_IMAGE_FILE_FORMAT_JPEG_H_

#include "cve/src/core/image.h"
#include "cve/src/imageio/image_file_format.h"

namespace cve
{
    namespace jpeg
    {
        constexpr uint16 JFIF_HEADER_LENGTH = 16;

        enum class MarkerId : uint8
        {
            StartOfFile = 0xD8,
            AppDefaultHeader = 0xE0,
            QuantizationTable = 0xDB,
            StartOfFrame = 0xC0,
            HuffmanTable = 0xC4,
            StartOfScan = 0xDA,
            EndOfFile = 0xD9
        };

        constexpr std::array<uint8, 64> QuantizationTableLuminance =
            { 16, 11, 10, 16, 24, 40, 51, 61, // there are a few experts proposing slightly more efficient values,
              12, 12, 14, 19, 26, 58, 60, 55, // e.g. https://www.imagemagick.org/discourse-server/viewtopic.php?t=20333
              14, 13, 16, 24, 40, 57, 69, 56, // btw: Google's Guetzli project optimizes the quantization tables per image
              14, 17, 22, 29, 51, 87, 80, 62,
              18, 22, 37, 56, 68,109,103, 77,
              24, 35, 55, 64, 81,104,113, 92,
              49, 64, 78, 87,103,121,120,101,
              72, 92, 95, 98,112,100,103, 99 };
        constexpr std::array<uint8, 64> QuantizationTableChrominance =
            { 17, 18, 24, 47, 99, 99, 99, 99,
              18, 21, 26, 66, 99, 99, 99, 99,
              24, 26, 56, 99, 99, 99, 99, 99,
              47, 66, 99, 99, 99, 99, 99, 99,
              99, 99, 99, 99, 99, 99, 99, 99,
              99, 99, 99, 99, 99, 99, 99, 99,
              99, 99, 99, 99, 99, 99, 99, 99,
              99, 99, 99, 99, 99, 99, 99, 99 };

        /** Table of backward zigzags for encoding. */
        constexpr std::array<uint8, 64> ZigZagBackward =
            {  0, 1, 8,16, 9, 2, 3,10,
              17,24,32,25,18,11, 4, 5,
              12,19,26,33,40,48,41,34,
              27,20,13, 6, 7,14,21,28,
              35,42,49,56,57,50,43,36,
              29,22,15,23,30,37,44,51,
              58,59,52,45,38,31,39,46,
              53,60,61,54,47,55,62,63 };

        /** Table of forward zigzags for decoding. */
        constexpr std::array<uint8, 64> ZigZagForward[] =
            {  0, 1, 5, 6,14,15,27,28,
               2, 4, 7,13,16,26,29,42,
               3, 8,12,17,25,30,41,43,
               9,11,18,24,31,40,44,53,
              10,19,23,32,39,45,52,54,
              20,22,33,38,46,51,55,60,
              21,34,37,47,50,56,59,61,
              35,36,48,49,57,58,62,63 };

        template<typename Float>
        struct DiscreteCosineTransform
        {
            enum class Mode : uint8
            {
                Horizontal = 1,
                Vertical= 8
            }

            /** Computes the discrete cosine transform in forward mode on the given 8x8 block of data
              *
              * From the toojpeg library at https://github.com/stbrumme/toojpeg
              * base on  Fast AAN algorithm by Arai, Agui and Nakajima: "A fast DCT-SQ scheme for images" */
            void forward(Float block[8*8], const Mode mode) const
            {
              constexpr auto SqrtHalfSqrt = Float{1.306562965f}; //    sqrt((2 + sqrt(2)) / 2) = cos(pi * 1 / 8) * sqrt(2)
              constexpr auto InvSqrt      = Float{0.707106781f}; // 1 / sqrt(2)                = cos(pi * 2 / 8)
              constexpr auto HalfSqrtSqrt = Float{0.382683432f}; //     sqrt(2 - sqrt(2)) / 2  = cos(pi * 3 / 8)
              constexpr auto InvSqrtSqrt  = Float{0.541196100f}; // 1 / sqrt(2 - sqrt(2))      = cos(pi * 3 / 8) * sqrt(2)

              const auto stride = static_cast<uint8>(mode);

              // modify in-place
              auto& block0 = block[0         ];
              auto& block1 = block[1 * stride];
              auto& block2 = block[2 * stride];
              auto& block3 = block[3 * stride];
              auto& block4 = block[4 * stride];
              auto& block5 = block[5 * stride];
              auto& block6 = block[6 * stride];
              auto& block7 = block[7 * stride];

              // based on https://dev.w3.org/Amaya/libjpeg/jfdctflt.c , the original variable names can be found in my comments
              auto add07 = block0 + block7; auto sub07 = block0 - block7; // tmp0, tmp7
              auto add16 = block1 + block6; auto sub16 = block1 - block6; // tmp1, tmp6
              auto add25 = block2 + block5; auto sub25 = block2 - block5; // tmp2, tmp5
              auto add34 = block3 + block4; auto sub34 = block3 - block4; // tmp3, tmp4

              auto add0347 = add07 + add34; auto sub07_34 = add07 - add34; // tmp10, tmp13 ("even part" / "phase 2")
              auto add1256 = add16 + add25; auto sub16_25 = add16 - add25; // tmp11, tmp12

              block0 = add0347 + add1256; block4 = add0347 - add1256; // "phase 3"

              auto z1 = (sub16_25 + sub07_34) * InvSqrt; // all temporary z-variables kept their original names
              block2 = sub07_34 + z1; block6 = sub07_34 - z1; // "phase 5"

              auto sub23_45 = sub25 + sub34; // tmp10 ("odd part" / "phase 2")
              auto sub12_56 = sub16 + sub25; // tmp11
              auto sub01_67 = sub16 + sub07; // tmp12

              auto z5 = (sub23_45 - sub01_67) * HalfSqrtSqrt;
              auto z2 = sub23_45 * InvSqrtSqrt  + z5;
              auto z3 = sub12_56 * InvSqrt;
              auto z4 = sub01_67 * SqrtHalfSqrt + z5;
              auto z6 = sub07 + z3; // z11 ("phase 5")
              auto z7 = sub07 - z3; // z13
              block1 = z6 + z4; block7 = z6 - z4; // "phase 6"
              block5 = z7 + z2; block3 = z7 - z2;
            }
        };
    }

    struct JPEG
    {
        /** Quality of the JPEG file. Must lie in [1;100] */
        uint32 quality = uint32{1};
    };


    template<>
    class ImageReader<JPEG>
    {
    public:
        template<typename ColorSpace>
        void operator()(std::istream &stream, Image<ColorSpace> &img) const
        {

        }
    };

    template<>
    class ImageWriter<JPEG>
    {
    public:
        template<typename ColorSpace>
        void operator()(std::ostream &stream, const Image<ColorSpace> &img, const JPEG &options) const
        {
            if(!writeJFIFHeader(stream))
                throw std::runtime_error("failed to write JFIF header");

            std::array<char, 64> luminanceQuantization;
            std::array<char, 64> chrominanceQuantization;
            if(!writeQuantizationTables(stream, luminanceQuantization, chrominanceQuantization, options.quality))
                throw std::runtime_error("failed to write quantization tables")

            if(!writeImageInfos(stream, img.cols(), img.rows()))
                throw std::runtime_error("failed to write image infos")


            if(!writeMarker(stream, jpeg::MarkerId::EndOfFile))
                throw std::runtime_error("failed to write end of file")

        }
    private:
        bool writeJFIFHeader(std::ostream &stream) const
        {
            if(!writeMarker(stream, jpeg::MarkerId::StartOfFile))
                return false;

            if(!writeMarker(stream, jpeg::MarkerId::AppDefaultHeader, jpeg::JFIF_HEADER_LENGTH))
                return false;

            stream.put('J');
            stream.put('F');
            stream.put('I');
            stream.put('F');
            stream.put(0);

            // version 1.1
            stream.put(1);
            stream.put(1);

            // pixel density
            stream.put(0);
            if(!(writeUInt16(stream, 1) && writeUInt16(stream, 1)))
                return false;

            // thumbnail
            stream.put(0);
            stream.put(0);

            return stream.good();
        }

        bool writeQuantizationTables(std::ostream &stream, std::array<char, 64> &luminanceQuantization, std::array<char, 64> &chrominanceQuantization, uint32 quality) const
        {
            // quality level must be in 1 ... 100
            quality = clamp<uint32>(quality, 1, 100);
            // convert to an internal JPEG quality factor, formula taken from libjpeg
            auto qualityFactor = quality < 50 ? 5000 / quality : 200 - quality * 2;

            for (std::size_t i = 0; i < 64; ++i)
            {
                uint32 luminance = (jpeg::QuantizationTableLuminance[jpeg::ZigZagBackward[i]] * qualityFactor + 50) / 100;
                uint32 chrominance = (jpeg::QuantizationTableChrominance[jpeg::ZigZagBackward[i]] * qualityFactor + 50) / 100;

                luminanceQuantization[i] = static_cast<char>(clamp<uint32>(luminance,   1, 255));
                chrominanceQuantization[i] = static_cast<char>(clamp<uint32>(chrominance, 1, 255));
            }

            if(!writeMarker(stream, jpeg::MarkerId::QuantizationTable, 2 + 2 * 65))
                return false;

            stream.put(0x00);
            stream.write(luminanceQuantization.data(), luminanceQuantization.size());
            if(!stream.good())
                return false;

            stream.put(0x01);
            stream.write(chrominanceQuantization.data(), chrominanceQuantization.size());
            if(!stream.good())
                return false;

            return true;
        }

        bool writeImageInfos(std::ostream &stream, uint16 width, uint16 height) const
        {
            const auto channels = uint8{3};
            // length = 2bytes for length, 6 bytes for general infos, 3 bytes for each channel
            if(!writeMarker(stream, jpeg::MarkerId::StartOfFrame, 2 + 6 + 3 * channels))
                return false;

            // 8 bit per channel
            stream.put(8);

            // width and height of the image
            if(!writeUInt16(stream, width) || !writeUInt16(stream, height))
                return false;

            // number of channels
            stream.put(channels);
            if(!stream.good())
                return false;

            for(std::size_t i = 0; i < channels; ++i)
            {
                // id of the channel:
                // 1 = Y, 2 = Cb, 3 = Cr
                stream.put(i + 1);

                // no downsampling for any channel
                stream.put(0x11);

                // id of the quantization table to use
                stream.put(i == 0 ? 0 : 1);

                if(!stream.good())
                    return false;
            }

            return true;
        }

        bool writeMarker(std::ostream &stream, const jpeg::MarkerId marker) const
        {
            stream.put(0xFF);
            stream.put(static_cast<uint8>(marker));
            return stream.good();
        }

        bool writeMarker(std::ostream &stream, const jpeg::MarkerId marker, const uint16 length) const
        {
            if(!writeMarker(stream, marker))
                return false;

            return writeUInt16(stream, length);
        }

        bool writeUInt32(std::ostream &stream, const uint32 value) const
        {
            stream.put(static_cast<uint8>(value & 0x000000FF));
            stream.put(static_cast<uint8>((value >> 8) & 0x000000FF));
            stream.put(static_cast<uint8>((value >> 16) & 0x000000FF));
            stream.put(static_cast<uint8>((value >> 24) & 0x000000FF));
            return stream.good();
        }

        bool writeUInt16(std::ostream &stream, const uint16 value) const
        {
            stream.put(static_cast<uint8>(value & 0x00FF));
            stream.put(static_cast<uint8>((value >> 8) & 0x00FF));
            return stream.good();
        }


    };
}

#endif