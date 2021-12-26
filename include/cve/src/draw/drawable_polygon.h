/* drawable_polygon.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_DRAWABLE_POLYGON_H_
#define CVE_DRAWABLE_POLYGON_H_

#include "cve/src/draw/drawable_types.h"
#include "cve/src/core/image.h"
#include "cve/src/draw/color.h"

namespace cve
{
    struct Polygon
    {
        using Scalar = cve::float32;
        using Vector2 = Eigen::Matrix<float32, 2, 1>;

        std::vector<Vector2> points;
        FillMode fillMode = FillMode::Outline;

        Polygon() = default;

        Polygon(const std::vector<Vector2> &points, const FillMode fillMode = FillMode::Outline)
            : points(points), fillMode(fillMode)
        { }

    };

    namespace image
    {
        namespace internal
        {
            template<typename ColorSpace>
            inline void drawOutlined(Image<ColorSpace> &, const Polygon &, const Color<ColorSpace> &)
            {
            }

            template<typename ColorSpace>
            inline void drawFilled(Image<ColorSpace> &img, const Polygon &polygon, const Color<ColorSpace> &color)
            {
                using Point2 = Eigen::Matrix<Index, 2, 1>;
                using Line2 = Eigen::Matrix<cve::float32, 2, 1>;

                const auto pixel = cve::color::toPixel(color);

                // compute bounding box
                auto topLeft = Point2(img.cols(), img.rows());
                auto botRight = Point2(0, 0);
                const auto count = static_cast<Index>(polygon.points.size());
                auto lines = std::vector<Line2>(count);

                for(Index i = 0; i < count; ++i)
                {
                    const auto &pointA = polygon.points[i];
                    const auto &pointB = polygon.points[(i+1) % count];

                    topLeft(0) = std::min(topLeft(0), static_cast<Index>(std::floor(pointA(0))));
                    topLeft(1) = std::min(topLeft(1), static_cast<Index>(std::floor(pointA(1))));
                    botRight(0) = std::max(botRight(0), static_cast<Index>(std::ceil(pointA(0))));
                    botRight(1) = std::max(botRight(1), static_cast<Index>(std::ceil(pointA(1))));

                    const auto slope = (pointB.y() - pointA.y()) / (pointB.x() - pointA.x());
                    const auto offset = pointA.y() - slope * pointA.x();
                    lines[i] << slope, offset;
                }

                for(Index scanline = topLeft.y(); scanline <= botRight.y(); ++scanline)
                {
                    const auto scanlinef = static_cast<cve::float32>(scanline);
                    std::vector<cve::float32> intersection;
                    for(Index i = 0; i < count; ++i)
                    {
                        const auto &pointA = polygon.points[i];
                        const auto &pointB = polygon.points[(i+1) % count];
                        const auto ymax = std::max(pointA.y(), pointB.y());
                        const auto ymin = std::min(pointA.y(), pointB.y());
                        if(ymin <= scanlinef && ymax > scanlinef)
                        {
                            const auto slope = lines[i](0);
                            const auto offset = lines[i](1);
                            intersection.push_back((scanlinef - offset) / slope);
                        }
                    }

                    assert(intersection.size() % 2 == 0);
                    std::sort(intersection.begin(), intersection.end());

                    for(size_t i = 0; i < intersection.size(); i += 2)
                    {
                        const auto leftx = static_cast<Index>(std::ceil(intersection[i]));
                        const auto rightx = static_cast<Index>(std::ceil(intersection[i+1]));

                        for(Index x = leftx; x < rightx; ++x)
                        {
                            if(image::inside(img, scanline, x))
                            {
                                img(scanline, x) = pixel;
                            }
                        }
                    }
                }
            }
        }

        template<typename ColorSpace>
        inline void draw(Image<ColorSpace> &img, const Polygon &polygon, const Color<ColorSpace> &color)
        {
            switch(polygon.fillMode)
            {
                case FillMode::Fill:
                    internal::drawFilled(img, polygon, color);
                    break;
                case FillMode::Outline:
                    internal::drawOutlined(img, polygon, color);
                    break;
            }
        }
    }
}

#endif