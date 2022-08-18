/* drawable_circle.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef NVISION_DRAWABLE_CIRCLE_H_
#define NVISION_DRAWABLE_CIRCLE_H_

#include "nvision/src/draw/drawable_types.h"
#include "nvision/src/core/image.h"
#include "nvision/src/draw/color.h"

namespace nvision
{
    struct Circle
    {
        using Scalar = nvision::float32;
        using Vector2 = Eigen::Matrix<float32, 2, 1>;

        Vector2 center{0, 0};
        float32 radius = 1;
        FillMode fillMode = FillMode::Outline;

        Circle() = default;

        Circle(const Eigen::Matrix<float32, 2, 1> &center, const float32 radius)
            : center(center), radius(radius)
        { }

        Circle(const Eigen::Matrix<float32, 2, 1> &center, const float32 radius, const FillMode fillMode)
            : center(center), radius(radius), fillMode(fillMode)
        { }
    };

    namespace image
    {
        namespace internal
        {
            template<typename ColorSpace>
            void drawOutlined(Image<ColorSpace> &img, const Circle &circle, const Color<ColorSpace> &color)
            {
                const auto cx = circle.center(0);
                const auto cy = circle.center(1);

                auto px = circle.radius;
                auto py = float32{0};

                const auto cxInt = static_cast<Index>(std::round(cx));
                const auto cyInt = static_cast<Index>(std::round(cy));

                const auto pixel = nvision::color::toPixel(color);

                while(px >= py)
                {
                    const auto pxInt = static_cast<Index>(std::round(px));
                    const auto pyInt = static_cast<Index>(std::round(py));

                    auto x = cxInt + pxInt;
                    auto y = cyInt + pyInt;
                    if(x >= 0 && x < img.cols() &&
                       y >= 0 && y < img.rows())
                    {
                        img(y, x) = nvision::color::toPixel(color);
                    }

                    x = cxInt + pxInt;
                    y = cyInt - pyInt;
                    if(x >= 0 && x < img.cols() &&
                       y >= 0 && y < img.rows())
                    {
                        img(y, x) = pixel;
                    }

                    x = cxInt - pxInt;
                    y = cyInt + pyInt;
                    if(x >= 0 && x < img.cols() &&
                       y >= 0 && y < img.rows())
                    {
                        img(y, x) = pixel;
                    }

                    x = cxInt - pxInt;
                    y = cyInt - pyInt;
                    if(x >= 0 && x < img.cols() &&
                       y >= 0 && y < img.rows())
                    {
                        img(y, x) = pixel;
                    }

                    x = cxInt + pyInt;
                    y = cyInt + pxInt;
                    if(x >= 0 && x < img.cols() &&
                       y >= 0 && y < img.rows())
                    {
                        img(y, x) = pixel;
                    }

                    x = cxInt + pyInt;
                    y = cyInt - pxInt;
                    if(x >= 0 && x < img.cols() &&
                       y >= 0 && y < img.rows())
                    {
                        img(y, x) = pixel;
                    }

                    x = cxInt - pyInt;
                    y = cyInt + pxInt;
                    if(x >= 0 && x < img.cols() &&
                       y >= 0 && y < img.rows())
                    {
                        img(y, x) = pixel;
                    }

                    x = cxInt - pyInt;
                    y = cyInt - pxInt;
                    if(x >= 0 && x < img.cols() &&
                       y >= 0 && y < img.rows())
                    {
                        img(y, x) = pixel;
                    }

                    px = std::sqrt(px * px - 2 * py - 1);
                    py = std::sqrt(py * py + 2 * py + 1);
                }
            }

            template<typename ColorSpace>
            void drawFilled(Image<ColorSpace> &img, const Circle &circle, const Color<ColorSpace> &color)
            {
                using Point2 = Eigen::Matrix<Index, 2, 1>;

                const auto pixel = nvision::color::toPixel(color);

                // compute bounding box
                const auto topLeft = Point2(static_cast<Index>(std::floor(circle.center.x() - circle.radius)),
                                            static_cast<Index>(std::floor(circle.center.y() - circle.radius)));
                const auto botRight = Point2(static_cast<Index>(std::ceil(circle.center.x() + circle.radius)),
                                            static_cast<Index>(std::ceil(circle.center.y() + circle.radius)));

                for(Index c = topLeft.x(); c < botRight.x(); ++c)
                {
                    for(Index r = topLeft.y(); r < botRight.y(); ++r)
                    {
                        const auto dx = static_cast<nvision::float32>(c) - circle.center.x();
                        const auto dy = static_cast<nvision::float32>(r) - circle.center.y();
                        const auto dist = dx * dx + dy * dy;
                        if(dist <= circle.radius * circle.radius && image::inside(img, r, c))
                        {
                            img(r, c) = pixel;
                        }
                    }
                }
            }
        }

        template<typename ColorSpace>
        void draw(Image<ColorSpace> &img, const Circle &circle, const Color<ColorSpace> &color)
        {
            switch(circle.fillMode)
            {
                case FillMode::Fill:
                    internal::drawFilled(img, circle, color);
                    break;
                case FillMode::Outline:
                    internal::drawOutlined(img, circle, color);
                    break;
            }
        }
    }
}

#endif
