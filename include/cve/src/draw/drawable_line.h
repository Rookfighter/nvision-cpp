/* drawable_line.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_DRAWABLE_LINE_H_
#define CVE_DRAWABLE_LINE_H_

#include "cve/src/core/image.h"
#include "cve/src/draw/color.h"

namespace cve
{
    struct Line
    {
        using Vector2 = const Eigen::Matrix<float32, 2, 1>;
        Vector2 p0{0, 0};
        Vector2 p1{1, 1};

        Line() = default;
        Line(const Vector2 &p0, const Vector2 &p1)
            : p0(p0), p1(p1)
        { }
        Line(const float32 x0, const float32 y0, const float32 x1, const float32 y1)
            : p0(x0, y0), p1(x1, y1)
        { }
    };

    namespace image
    {
        namespace internal
        {
            template<typename ColorSpace>
            void drawLow(Image<ColorSpace> &img,
                const Index x0,
                const Index y0,
                const Index x1,
                const Index y1,
                const Color<ColorSpace> &color)
            {
                const auto pixel = cve::color::toPixel(color);
                const auto dx = x1 - x0;
                auto dy = y1 - y0;
                auto yi = Index{1};
                if(dy < 0)
                {
                    yi = Index{-1};
                    dy = -dy;
                }

                auto D = 2 * dy - dx;
                auto y = y0;
                for(auto x = x0; x <= x1; x += 1)
                {
                    if(image::inside(img, y, x))
                        img(y, x) = pixel;

                    if(D > 0)
                    {
                        y = y + yi;
                        D = D + 2 * (dy- dx);
                    }
                    else
                    {
                        D = D + 2 * dy;
                    }
                }
            }

            template<typename ColorSpace>
            void drawHigh(Image<ColorSpace> &img,
                          const Index x0,
                          const Index y0,
                          const Index x1,
                          const Index y1,
                          const Color<ColorSpace> &color)
            {
                const auto pixel = cve::color::toPixel(color);
                const auto dy = y1 - y0;
                auto dx = x1 - x0;
                auto xi = Index{1};
                if(dx < 0)
                {
                    xi = Index{-1};
                    dx = -dx;
                }

                auto D = 2 * dx - dy;
                auto x = x0;
                for(auto y = y0; y <= y1; y += 1)
                {
                    if(image::inside(img, y, x))
                        img(y, x) = pixel;

                    if(D > 0)
                    {
                        x = x + xi;
                        D = D + 2 * (dx - dy);
                    }
                    else
                    {
                        D = D + 2 * dx;
                    }
                }
            }
        }

        template<typename ColorSpace>
        void draw(Image<ColorSpace> &img, const Line &line, const Color<ColorSpace> &color)
        {
            const auto x0 = static_cast<Index>(std::round(line.p0.x()));
            const auto y0 = static_cast<Index>(std::round(line.p0.y()));
            const auto x1 = static_cast<Index>(std::round(line.p1.x()));
            const auto y1 = static_cast<Index>(std::round(line.p1.y()));
            if(std::abs(y1 - y0) < std::abs(x1 - x0))
            {
                if(x0 > x1)
                    internal::drawLow(img, x1, y1, x0, y0, color);
                else
                    internal::drawLow(img, x0, y0, x1, y1, color);
            }
            else
            {
                if(y0 > y1)
                    internal::drawHigh(img, x1, y1, x0, y0, color);
                else
                    internal::drawHigh(img, x0, y0, x1, y1, color);
            }
        }
    }
}

#endif