/* color_space_bgra.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Dec 2019
 */

#ifndef NVISION_COLOR_SPACE_BGRA_H_
#define NVISION_COLOR_SPACE_BGRA_H_

#include "nvision/src/core/color_space_bgr.h"
#include "nvision/src/core/color_space_rgba.h"

namespace nvision
{
    /** BGRA color space with float32 types. */
    struct BGRAf
    {
        using ValueType = float32;
        static constexpr Index Dimension = 4;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {1, 1, 1, 1};
    };

    template<>
    struct IsColorSpace<BGRAf>
    {
        static constexpr bool value = true;
    };

    /** BGRA color space with byte types. */
    struct BGRA
    {
        using ValueType = uint8;
        static constexpr Index Dimension = 4;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {255, 255, 255, 255};
    };

    template<>
    struct IsColorSpace<BGRA>
    {
        static constexpr bool value = true;
    };

    // BGRAf
    namespace pixel
    {
        template<>
        inline const Pixel<BGRAf> convert(const Pixel<BGRAf> &from)
        {
            return from;
        }

        template<>
        inline const Pixel<BGRAf> convert(const Pixel<RGBf> &from)
        {
            return Pixel<BGRAf>{from[2], from[1], from[0], 1.0f};
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<BGRAf> &from)
        {
            return Pixel<RGBf>{from[2], from[1], from[0]};
        }

        template<>
        inline const Pixel<BGRAf> convert(const Pixel<BGRf> &from)
        {
            return Pixel<BGRAf>{from[0], from[1], from[2], 1.0f};
        }

        template<>
        inline const Pixel<BGRf> convert(const Pixel<BGRAf> &from)
        {
            return Pixel<BGRf>{from[0], from[1], from[2]};
        }

        template<>
        inline const Pixel<BGRAf> convert(const Pixel<RGBAf> &from)
        {
            return Pixel<BGRAf>{from[2], from[1], from[0], from[3]};
        }

        template<>
        inline const Pixel<RGBAf> convert(const Pixel<BGRAf> &from)
        {
            return Pixel<RGBAf>{from[2], from[1], from[0], from[3]};
        }

        template<>
        inline const Pixel<BGRAf> convert(const Pixel<RGBA> &from)
        {
            return convert<RGBAf, BGRAf>(convert<RGBA, RGBAf>(from));
        }

        template<>
        inline const Pixel<RGBA> convert(const Pixel<BGRAf> &from)
        {
            return convert<RGBAf, RGBA>(convert<BGRAf, RGBAf>(from));
        }
    }

    // BGRA
    namespace pixel
    {
        template<>
        inline const Pixel<BGRA> convert(const Pixel<BGRA> &from)
        {
            return from;
        }

        template<>
        inline const Pixel<BGRA> convert(const Pixel<BGRAf> &from)
        {
            return convertRange<BGRAf, BGRA>(from);
        }

        template<>
        inline const Pixel<BGRAf> convert(const Pixel<BGRA> &from)
        {
            return convertRange<BGRA, BGRAf>(from);
        }

        template<>
        inline const Pixel<BGRA> convert(const Pixel<RGBf> &from)
        {
            return convert<BGRAf, BGRA>(convert<RGBf, BGRAf>(from));
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<BGRA> &from)
        {
            return convert<BGRAf, RGBf>(convert<BGRA, BGRAf>(from));
        }

        template<>
        inline const Pixel<BGRA> convert(const Pixel<BGR> &from)
        {
            return Pixel<BGRA>{from[0], from[1], from[2], 255};
        }

        template<>
        inline const Pixel<BGR> convert(const Pixel<BGRA> &from)
        {
            return Pixel<BGR>{from[0], from[1], from[2]};
        }

        template<>
        inline const Pixel<BGRA> convert(const Pixel<RGB> &from)
        {
            return convert<BGR, BGRA>(convert<RGB, BGR>(from));
        }

        template<>
        inline const Pixel<RGB> convert(const Pixel<BGRA> &from)
        {
            return convert<BGR, RGB>(convert<BGRA, BGR>(from));
        }

        template<>
        inline const Pixel<BGRA> convert(const Pixel<RGBA> &from)
        {
            return Pixel<BGRA>{from[2], from[1], from[0], from[3]};
        }

        template<>
        inline const Pixel<RGBA> convert(const Pixel<BGRA> &from)
        {
            return Pixel<RGBA>{from[2], from[1], from[0], from[3]};
        }

        template<>
        inline const Pixel<BGRA> convert(const Pixel<RGBAf> &from)
        {
            return convert<BGRAf, BGRA>(convert<RGBAf, BGRAf>(from));
        }

        template<>
        inline const Pixel<RGBAf> convert(const Pixel<BGRA> &from)
        {
            return convert<BGRAf, RGBAf>(convert<BGRA, BGRAf>(from));
        }
    }

    template<>
    struct GetIntegralColorSpace<BGRAf>
    {
        using type = BGRA;
    };

    template<>
    struct GetIntegralColorSpace<BGRA>
    {
        using type = BGRA;
    };
}

#endif
