/* color_space_gray.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Dec 2019
 */

#ifndef NVISION_COLOR_SPACE_GRAY_H_
#define NVISION_COLOR_SPACE_GRAY_H_

#include "nvision/src/core/color_space_rgb.h"

namespace nvision
{
    /** Gray color space with float32 types. */
    struct Grayf
    {
        using ValueType = float32;
        static constexpr Index Dimension = 1;

        static constexpr std::array<ValueType, Dimension> minimum = {0};
        static constexpr std::array<ValueType, Dimension> maximum = {1};
    };

    template<>
    struct IsColorSpace<Grayf>
    {
        static constexpr bool value = true;
    };

    /** Gray color space with byte types. */
    struct Gray
    {
        using ValueType = uint8;
        static constexpr Index Dimension = 1;

        static constexpr std::array<ValueType, Dimension> minimum = {0};
        static constexpr std::array<ValueType, Dimension> maximum = {255};
    };

    template<>
    struct IsColorSpace<Gray>
    {
        static constexpr bool value = true;
    };

    // Grayf
    namespace pixel
    {
        template<>
        inline const Pixel<Grayf> convert(const Pixel<Grayf> &from)
        {
            return from;
        }

        template<>
        inline const Pixel<Grayf> convert(const Pixel<RGBf> &from)
        {
            const auto gray =   0.299f * static_cast<float32>(from[0])
                              + 0.587f * static_cast<float32>(from[1])
                              + 0.114f * static_cast<float32>(from[2]);
            return Pixel<Grayf>{gray};
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<Grayf> &from)
        {
            return Pixel<RGBf>{from[0], from[0], from[0]};
        }
    }

    // Gray
    namespace pixel
    {
        template<>
        inline const Pixel<Gray> convert(const Pixel<Gray> &from)
        {
            return from;
        }

        template<>
        inline const Pixel<Gray> convert(const Pixel<Grayf> &from)
        {
            return convertRange<Grayf, Gray>(from);
        }

        template<>
        inline const Pixel<Grayf> convert(const Pixel<Gray> &from)
        {
            return convertRange<Gray, Grayf>(from);
        }

        template<>
        inline const Pixel<Gray> convert(const Pixel<RGBf> &from)
        {
            return convert<Grayf, Gray>(convert<RGBf, Grayf>(from));
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<Gray> &from)
        {
            return convert<Grayf, RGBf>(convert<Gray, Grayf>(from));
        }

        template<>
        inline const Pixel<RGB> convert(const Pixel<Gray> &from)
        {
            return Pixel<RGB>(from[0], from[0], from[0]);
        }
    }

    template<>
    struct GetIntegralColorSpace<Grayf>
    {
        using type = Gray;
    };

    template<>
    struct GetIntegralColorSpace<Gray>
    {
        using type = Gray;
    };
}

#endif
