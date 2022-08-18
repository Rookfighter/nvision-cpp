/* color_space_hsv.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Dec 2019
 */

#ifndef NVISION_COLOR_SPACE_HSV_H_
#define NVISION_COLOR_SPACE_HSV_H_

#include "nvision/src/core/color_space_rgb.h"

namespace nvision
{
    /** HSV color space with float32 types. */
    struct HSV
    {
        using ValueType = float32;
        static constexpr Index Dimension = 3;

        static constexpr std::array<ValueType, Dimension> minimum = {0, 0, 0};
        static constexpr std::array<ValueType, Dimension> maximum = {1, 1, 1};
    };

    namespace pixel
    {
        template<>
        inline const Pixel<HSV> convert(const Pixel<HSV> &from)
        {
            return from;
        }

        /** Converts a RGB pixel to HSV.
          * See also https://en.wikipedia.org/wiki/HSL_and_HSV#From_RGB
          * @param from RGB pixel
          * @return HSV pixel. */
        template<>
        inline const Pixel<HSV> convert(const Pixel<RGBf> &from)
        {
            const auto r = from[0];
            const auto g = from[1];
            const auto b = from[2];

            const auto xmax = std::max(r, std::max(g, b));
            const auto xmin = std::min(r, std::min(g, b));

            const auto v = xmax;
            const auto c = xmax - xmin;
            const auto s = v == float32{0} ? float32{0} : c / v;

            auto h = float32{0};
            if(float32{0} == c)
                h = float32{0};
            else if(v == r)
                h = std::fmod((g - b) / c, float32{6});
            else if(v == g)
                h = (b - r) / c + float32{2};
            else if(v == b)
                h = (r - g) / c + float32{4};

            h *= float32{60};

            // map hue on interval [0;1]
            h /= float32{360};

            return Pixel<HSV>(h, s, v);
        }

        /** Converts a HSV pixel to RGB.
          * See also https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB_alternative
          * @param from HSV pixel
          * @return RGB pixel. */
        template<>
        inline const Pixel<RGBf> convert(const Pixel<HSV> &from)
        {
            const auto h = from[0] * float32{360};
            const auto s = from[1];
            const auto v = from[2];

            const auto f = [h, s, v](const float32 n) -> float32
            {
                const auto k = std::fmod(n + h / float32{60}, float32{6});
                return v - v * s * std::max(float32{0}, std::min(k, std::min(float32{4} - k, float32{1})));
            };

            const auto r = f(float32{5});
            const auto g = f(float32{3});
            const auto b = f(float32{1});

            return Pixel<RGBf>(r, g, b);
        }
    }
}

#endif
