/* color_space_bgr.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Dec 2019
 */

#ifndef NVISION_COLOR_SPACE_BGR_H_
#define NVISION_COLOR_SPACE_BGR_H_

#include "nvision/src/core/color_space_rgb.h"

namespace nvision
{
    /** BGR color space with float32 types. */
    struct BGRf
    {
        using ValueType = float32;
        static constexpr Index Dimension = 3;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {1, 1, 1};
    };

    template<>
    struct IsColorSpace<BGRf>
    {
        static constexpr bool value = true;
    };

    /** BGR color space with byte types. */
    struct BGR
    {
        using ValueType = uint8;
        static constexpr Index Dimension = 3;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {255, 255, 255};
    };

    template<>
    struct IsColorSpace<BGR>
    {
        static constexpr bool value = true;
    };

    // BGRf
    namespace pixel
    {
        template<>
        inline const Pixel<BGRf> convert(const Pixel<BGRf> &from)
        {
            return from;
        }

        template<>
        inline const Pixel<BGRf> convert(const Pixel<RGBf> &from)
        {
            return Pixel<BGRf>{from[2], from[1], from[0]};
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<BGRf> &from)
        {
            return Pixel<RGBf>{from[2], from[1], from[0]};
        }
    }

    // BGR
    namespace pixel
    {
        template<>
        inline const Pixel<BGR> convert(const Pixel<BGR> &from)
        {
            return from;
        }

        template<>
        inline const Pixel<BGR> convert(const Pixel<BGRf> &from)
        {
            return convertRange<BGRf, BGR>(from);
        }

        template<>
        inline const Pixel<BGRf> convert(const Pixel<BGR> &from)
        {
            return convertRange<BGR, BGRf>(from);
        }

        template<>
        inline const Pixel<BGR> convert(const Pixel<RGBf> &from)
        {
            return convert<BGRf, BGR>(convert<RGBf, BGRf>(from));
        }

        template<>
        inline const Pixel<RGBf> convert(const Pixel<BGR> &from)
        {
            return convert<BGRf, RGBf>(convert<BGR, BGRf>(from));
        }

        template<>
        inline const Pixel<BGR> convert(const Pixel<RGB> &from)
        {
            return Pixel<BGR>{from[2], from[1], from[0]};
        }

        template<>
        inline const Pixel<RGB> convert(const Pixel<BGR> &from)
        {
            return Pixel<RGB>{from[2], from[1], from[0]};
        }
    }
}

#endif
