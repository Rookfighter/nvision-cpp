/* drawable_circle.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_DRAWABLE_MARKER_H_
#define CVE_DRAWABLE_MARKER_H_

#include "cve/src/draw/drawable_circle.h"
#include "cve/src/draw/drawable_rectangle.h"

namespace cve
{
    enum class Marker
    {
        Circle,
        Square,
        Triangle,
        Cross
    };

    namespace image
    {
        template<typename ColorSpace, typename Derived>
        void draw(Image<ColorSpace> &img, const Eigen::MatrixBase<Derived> &points, const Marker marker, const Color<ColorSpace> &color)
        {
            for(Index i = 0; i < points.cols(); ++i)
            {
                switch(marker)
                {
                    case Marker::Circle:

                        draw(img, Circle{points.col(i), 3}, color);
                        break;
                    case Marker::Square:
                        draw(img, Rectangle{points.col(i), Eigen::Matrix<float32, 2, 1>{6, 6}}, color);
                        break;
                    default:
                        break;
                }

            }
        }
    }
}

#endif