/* shape_drawer.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_SHAPE_DRAWER_H_
#define CVE_SHAPE_DRAWER_H_

#include "cve/image.h"

namespace cve
{
    template<typename Scalar>
    class ShapeDrawer
    {
    public:
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
        typedef Eigen::Matrix<Scalar, 3, 1> Vector3;
    public:

        template<typename ScalarA>
        void drawCircle(const Vector2 &center,
            const Scalar radius,
            const Eigen::Array<ScalarA, 3, 1> &color,
            Eigen::Tensor<ScalarA, 3> &img)
        {
            assert(img.dimension(2) >= 3);

            Scalar cx = center.x();
            Scalar cy = center.y();

            Scalar px = radius;
            Scalar py = 0;

            Index cxInt = std::round(cx);
            Index cyInt = std::round(cy);

            while(px >= py)
            {
                Index pxInt = std::round(px);
                Index pyInt = std::round(py);

                Index x = cxInt + pxInt;
                Index y = cyInt + pyInt;
                img(y, x, 0) = color(0);
                img(y, x, 1) = color(1);
                img(y, x, 2) = color(2);

                x = cxInt + pxInt;
                y = cyInt - pyInt;
                img(y, x, 0) = color(0);
                img(y, x, 1) = color(1);
                img(y, x, 2) = color(2);

                x = cxInt - pxInt;
                y = cyInt + pyInt;
                img(y, x, 0) = color(0);
                img(y, x, 1) = color(1);
                img(y, x, 2) = color(2);

                x = cxInt - pxInt;
                y = cyInt - pyInt;
                img(y, x, 0) = color(0);
                img(y, x, 1) = color(1);
                img(y, x, 2) = color(2);

                x = cxInt + pyInt;
                y = cyInt + pxInt;
                img(y, x, 0) = color(0);
                img(y, x, 1) = color(1);
                img(y, x, 2) = color(2);

                x = cxInt + pyInt;
                y = cyInt - pxInt;
                img(y, x, 0) = color(0);
                img(y, x, 1) = color(1);
                img(y, x, 2) = color(2);

                x = cxInt - pyInt;
                y = cyInt + pxInt;
                img(y, x, 0) = color(0);
                img(y, x, 1) = color(1);
                img(y, x, 2) = color(2);

                x = cxInt - pyInt;
                y = cyInt - pxInt;
                img(y, x, 0) = color(0);
                img(y, x, 1) = color(1);
                img(y, x, 2) = color(2);

                px = std::sqrt(px * px - 2 * py - 1);
                py = std::sqrt(py * py + 2 * py + 1);
            }
        }

        template<typename ScalarA>
        void drawRect(const Vector2 &topLeft,
            const Vector2 &dimensions,
            const Eigen::Array<ScalarA, 3, 1> &color,
            Eigen::Tensor<ScalarA, 3> &img)
        {
            Index width = dimensions(0);
            Index height = dimensions(1);

            Index px = topLeft(0);
            Index py = topLeft(1);

            for(Index x = px; x <= px + width; ++x)
            {
                img(py, x, 0) = color(0);
                img(py, x, 1) = color(1);
                img(py, x, 2) = color(2);

                img(py + height, x, 0) = color(0);
                img(py + height, x, 1) = color(1);
                img(py + height, x, 2) = color(2);
            }

            for(Index y = py; y <= py + height; ++y)
            {
                img(y, px, 0) = color(0);
                img(y, px, 1) = color(1);
                img(y, px, 2) = color(2);

                img(y, px + width, 0) = color(0);
                img(y, px + width, 1) = color(1);
                img(y, px + width, 2) = color(2);
            }
        }



    };
}

#endif
