/// @file
/// @author Fabian Meyer
/// @date 09 Dec 2019

#ifndef NVISION_COLOR_SPACE_RGB_H_
#define NVISION_COLOR_SPACE_RGB_H_

#include <array>
#include <nvision/core/types.h>
#include "nvision/color_space/traits.h"

namespace nvision
{
    /** RGB color space with float32 types. */
    struct RGBf
    {
        using ValueType = float32;
        static constexpr Index dimension = 3;

        static constexpr std::array<ValueType, dimension> minimum = {0, 0, 0};
        static constexpr std::array<ValueType, dimension> maximum = {1, 1, 1};
    };

    template<>
    struct IsColorSpace<RGBf>
    {
        static constexpr bool value = true;
    };

    /** RGB color space with byte types. */
    struct RGB
    {
        using ValueType = uint8;
        static constexpr Index Dimension = 3;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {255, 255, 255};
    };

    template<>
    struct IsColorSpace<RGB>
    {
        static constexpr bool value = true;
    };

    // RGBf
    namespace pixel
    {
        /** Converts any color space to any other color space if no explict
          * conversion is definied.
          * This implementation converts the source pixel to RGBf and then from RGBf
          * to the destination colorspace. */
        template<typename From, typename To>
        inline const Pixel<To> convert(const Pixel<From> &from)
        {
            static_assert(!std::is_same<From, RGBf>::value, "No conversion of From type to RGBf avaliable");
            static_assert(!std::is_same<To, RGBf>::value, "No conversion of RGBf to To type avaliable");
            return convert<RGBf, To>(convert<From, RGBf>(from));
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<RGBf> &from)
        {
            return from;
        }
    }

    // RGB
    namespace pixel
    {
        template<>
        inline const Pixel<RGB> convert(const Pixel<RGB> &from)
        {
            return from;
        }

        template<>
        inline const Pixel<RGB> convert(const Pixel<RGBf> &from)
        {
            return convertRange<RGBf, RGB>(from);
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<RGB> &from)
        {
            return convertRange<RGB, RGBf>(from);
        }
    }

    template<>
    struct GetIntegralColorSpace<RGBf>
    {
        using type = RGB;
    };

    template<>
    struct GetIntegralColorSpace<RGB>
    {
        using type = RGB;
    };
}

#endif
