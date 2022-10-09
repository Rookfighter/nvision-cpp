/* drawable_circle.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef NVISION_DRAWABLE_MARKER_H_
#define NVISION_DRAWABLE_MARKER_H_

#include "nvision/src/draw/drawable_circle.h"
#include "nvision/src/draw/drawable_rectangle.h"

namespace nvision
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
        template<typename DerivedImage, typename DerivedMatrix>
        void draw(ImageBase<DerivedImage> &img,
                  const Eigen::MatrixBase<DerivedMatrix> &points,
                  const Marker marker,
                  const Color<typename ImageBase<DerivedImage>::Scalar::ColorSpace> &color)
        {
            static_assert(IsImage<ImageBase<DerivedImage>>::value, "image must be of image type");
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
