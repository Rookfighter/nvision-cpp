/* color_space_rgba.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Dec 2019
 */

#ifndef NVISION_COLOR_SPACE_RGBA_H_
#define NVISION_COLOR_SPACE_RGBA_H_

#include "nvision/src/core/color_space_rgb.h"

namespace nvision
{
    /** RGBA color space with float32 types. */
    struct RGBAf
    {
        using ValueType = float32;
        static constexpr Index Dimension = 4;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {1, 1, 1, 1};
    };

    /** RGBA color space with byte types. */
    struct RGBA
    {
        using ValueType = uint8;
        static constexpr Index Dimension = 4;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {255, 255, 255, 255};
    };

    // RGBAf
    namespace pixel
    {
        template<>
        inline const Pixel<RGBAf> convert(const Pixel<RGBAf> &from)
        {
            return from;
        }

        template<>
        inline const Pixel<RGBAf> convert(const Pixel<RGBf> &from)
        {
            return Pixel<RGBAf>{from[0], from[1], from[2], 1.0f};
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<RGBAf> &from)
        {
            return Pixel<RGBf>{from[0], from[1], from[2]};
        }
    }

    // RGBA
    namespace pixel
    {
        template<>
        inline const Pixel<RGBA> convert(const Pixel<RGBA> &from)
        {
            return from;
        }

        template<>
        inline const Pixel<RGBA> convert(const Pixel<RGBAf> &from)
        {
            return convertRange<RGBAf, RGBA>(from);
        }

        template<>
        inline const Pixel<RGBAf> convert(const Pixel<RGBA> &from)
        {
            return convertRange<RGBA, RGBAf>(from);
        }

        template<>
        inline const Pixel<RGBA> convert(const Pixel<RGBf> &from)
        {
            return convert<RGBAf, RGBA>(convert<RGBf, RGBAf>(from));
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<RGBA> &from)
        {
            return convert<RGBAf, RGBf>(convert<RGBA, RGBAf>(from));
        }

        template<>
        inline const Pixel<RGBA> convert(const Pixel<RGB> &from)
        {
            return Pixel<RGBA>{from[0], from[1], from[2], 255};
        }

        template<>
        inline const Pixel<RGB> convert(const Pixel<RGBA> &from)
        {
            return Pixel<RGB>{from[0], from[1], from[2]};
        }
    }
}

#endif
