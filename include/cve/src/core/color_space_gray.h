/* color_space_gray.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Dec 2019
 */

#ifndef CVE_COLOR_SPACE_GRAY_H_
#define CVE_COLOR_SPACE_GRAY_H_

#include "cve/src/core/color_space_rgb.h"

namespace cve
{
    /** Gray color space with float32 types. */
    struct Grayf
    {
        using ValueType = float32;
        constexpr static Index Dimension = 1;

        constexpr static std::array<ValueType, Dimension> minimum = {0};
        constexpr static std::array<ValueType, Dimension> maximum = {1};
    };

    /** Gray color space with byte types. */
    struct Gray
    {
        using ValueType = uint8;
        constexpr static Index Dimension = 1;

        constexpr static std::array<ValueType, Dimension> minimum = {0};
        constexpr static std::array<ValueType, Dimension> maximum = {255};
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
}

#endif