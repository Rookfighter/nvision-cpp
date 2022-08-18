/* color_conversion.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef NVISION_COLOR_CONVERSION_H_
#define NVISION_COLOR_CONVERSION_H_

#include "nvision/src/draw/color_type.h"
#include "nvision/src/core/pixel.h"

namespace nvision
{
    namespace color
    {
        namespace internal
        {
            template<typename ColorSpace, Index Dimension>
            struct ColorPixelConverter { };

            template<typename ColorSpace>
            struct ColorPixelConverter<ColorSpace, 1>
            {
                Pixel<ColorSpace> toPixel(const Color<ColorSpace> &color) const
                {
                    return Pixel<ColorSpace>(color[0]);
                }

                Color<ColorSpace> fromPixel(const Pixel<ColorSpace> &pixel) const
                {
                    return Color<ColorSpace>(pixel[0]);
                }
            };

            template<typename ColorSpace>
            struct ColorPixelConverter<ColorSpace, 3>
            {
                Pixel<ColorSpace> toPixel(const Color<ColorSpace> &color) const
                {
                    return Pixel<ColorSpace>(color[0], color[1], color[2]);
                }

                Color<ColorSpace> fromPixel(const Pixel<ColorSpace> &pixel) const
                {
                    return Color<ColorSpace>(pixel[0], pixel[1], pixel[2]);
                }
            };

            template<typename ColorSpace>
            struct ColorPixelConverter<ColorSpace, 4>
            {
                Pixel<ColorSpace> toPixel(const Color<ColorSpace> &color) const
                {
                    return Pixel<ColorSpace>(color[0], color[1], color[2], color[3]);
                }

                Color<ColorSpace> fromPixel(const Pixel<ColorSpace> &pixel) const
                {
                    return Color<ColorSpace>(pixel[0], pixel[1], pixel[2], pixel[3]);
                }
            };
        }

        template<typename ColorSpace>
        inline Pixel<ColorSpace> toPixel(const Color<ColorSpace> &color)
        {
            internal::ColorPixelConverter<ColorSpace, ColorSpace::Dimension> converter;
            return converter.toPixel(color);
        }

        template<typename ColorSpace>
        inline Color<ColorSpace> fromPixel(const Pixel<ColorSpace> &pixel)
        {
            internal::ColorPixelConverter<ColorSpace, ColorSpace::Dimension> converter;
            return converter.fromPixel(pixel);
        }

        template<typename From, typename To>
        inline Color<To> convert(const Color<From> &from)
        {
            const auto pixelFrom = toPixel(from);
            const auto pixelTo = pixel::convert<From, To>(pixelFrom);
            return fromPixel(pixelTo);
        }
    }

    template<typename ColorSpace>
    template<typename TargetColorSpace>
    inline Color<TargetColorSpace> Color<ColorSpace>::convert() const
    {
        return color::convert<ColorSpace, TargetColorSpace>(*this);
    }
}

#endif
