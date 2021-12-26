/* drawable_rectangle.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_DRAWABLE_RECTANGLE_H_
#define CVE_DRAWABLE_RECTANGLE_H_

#include "cve/src/core/image.h"
#include "cve/src/draw/color.h"

namespace cve
{
    struct Rectangle
    {
        Eigen::Matrix<float32, 2, 1> topLeft{0, 0};
        Eigen::Matrix<float32, 2, 1> dimensions{1, 1};

        Rectangle() = default;
        Rectangle(const Eigen::Matrix<float32, 2, 1> &topLeft, const Eigen::Matrix<float32, 2, 1> &dimensions)
            : topLeft(topLeft), dimensions(dimensions)
        { }
    };

    namespace image
    {
        template<typename ColorSpace>
        void draw(Image<ColorSpace> &img, const Rectangle &rect, const Color<ColorSpace> &color)
        {
            const auto width = static_cast<Index>(std::round(rect.dimensions(0)));
            const auto height = static_cast<Index>(std::round(rect.dimensions(1)));

            const auto px = static_cast<Index>(std::round(rect.topLeft(0)));
            const auto py = static_cast<Index>(std::round(rect.topLeft(1)));

            const auto pixel = cve::color::toPixel(color);

            for(Index x = px; x <= px + width; ++x)
            {
                img(py, x) = pixel;
                img(py + height, x) = pixel;
            }

            for(Index y = py; y <= py + height; ++y)
            {
                img(y, px) = pixel;
                img(y, px + width) = pixel;
            }
        }
    }
}

#endif