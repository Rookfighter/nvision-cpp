/* color_space_ycbcr.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Dec 2019
 */

#ifndef NVISION_COLOR_SPACE_YCBCR_H_
#define NVISION_COLOR_SPACE_YCBCR_H_

#include "nvision/src/core/color_space_rgb.h"

namespace nvision
{
    /** YCbCr color space with float32 types. */
    struct YCbCrf
    {
        using ValueType = float32;
        static constexpr Index Dimension = 3;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {1, 1, 1};
    };

    template<>
    struct IsColorSpace<YCbCrf>
    {
        static constexpr bool value = true;
    };

    /** YCbCr color space with byte types. */
    struct YCbCr
    {
        using ValueType = uint8;
        static constexpr Index Dimension = 3;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {255, 255, 255};
    };

    template<>
    struct IsColorSpace<YCbCr>
    {
        static constexpr bool value = true;
    };

    // YCbCrf
    namespace pixel
    {
        template<>
        inline const Pixel<YCbCrf> convert(const Pixel<YCbCrf> &from)
        {
            return from;
        }

        /** Converts a RGB pixel to YCbCr.
          * See also https://en.wikipedia.org/wiki/YCbCr#JPEG_conversion
          * @param from RGB pixel
          * @return YCbCr pixel. */
        template<>
        inline const Pixel<YCbCrf> convert(const Pixel<RGBf> &from)
        {
            const auto r = from[0] * 255.0f;
            const auto g = from[1] * 255.0f;
            const auto b = from[2] * 255.0f;

            const auto y  = (0.299f * r + 0.587f * g + 0.114f * b) / 255.0f;
            const auto cb = (128.0f - 0.168736f * r - 0.331264f * g + 0.5f * b) / 255.0f;
            const auto cr = (128.0f + 0.5f * r - 0.418688f * g - 0.081312f * b) / 255.0f;

            return Pixel<YCbCrf>(y, cb, cr);
        }

        /** Converts a YCbCr pixel to RGB.
          * See also https://en.wikipedia.org/wiki/YCbCr#JPEG_conversion
          * @param from YCbCr pixel
          * @return RGB pixel. */
        template<>
        inline const Pixel<RGBf> convert(const Pixel<YCbCrf> &from)
        {
            const auto y = from[0];
            const auto cb = from[1];
            const auto cr = from[2];

            const auto r = y + 1.402f * (cr - 128.0f);
            const auto g = y - 0.344136f * (cb - 128.0f) - 0.714136f * (cr - 128.0f);
            const auto b = y + 1.772f * (cb - 128.0f);

            return Pixel<RGBf>(r, g, b);
        }
    }

    // YCbCr
    namespace pixel
    {
        template<>
        inline const Pixel<YCbCr> convert(const Pixel<YCbCr> &from)
        {
            return from;
        }

         template<>
        inline const Pixel<YCbCr> convert(const Pixel<YCbCrf> &from)
        {
            return convertRange<YCbCrf, YCbCr>(from);
        }

        template<>
        inline const Pixel<YCbCrf> convert(const Pixel<YCbCr> &from)
        {
            return convertRange<YCbCr, YCbCrf>(from);
        }

        template<>
        inline const Pixel<YCbCr> convert(const Pixel<RGBf> &from)
        {
            return convert<YCbCrf, YCbCr>(convert<RGBf, YCbCrf>(from));
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<YCbCr> &from)
        {
            return convert<YCbCrf, RGBf>(convert<YCbCr, YCbCrf>(from));
        }
    }
}

#endif
