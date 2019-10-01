/* bilinear_interpolation.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Sep 2019
 */

#ifndef CVE_BILINEAR_INTERPOLATION_H_
#define CVE_BILINEAR_INTERPOLATION_H_

#include <unsupported/Eigen/CXX11/Tensor>
#include "cve/core/matrix.h"
#include "cve/core/assert.h"

namespace cve
{
    /** Functor to compute bilinear interpolation on image values. */
    template<typename Scalar>
    class BilinearInterpolation
    {
    public:
        BilinearInterpolation()
        { }

        /** Compute the bilinear interpolation at the given image coordinates.
          * @param img source image used to compute the interpolation values
          * @param row row index of the interpolated pixel
          * @param col column index of the interpolated pixel
          * @param depth depth index of the interpolated pixel
          * @return interpolated pixel value */
        template<typename ScalarA>
        ScalarA operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const Scalar row,
            const Scalar col,
            const Index depth) const
        {
            CVE_ASSERT(depth >= 0 && depth < img.dimension(2));

            Scalar r1 = std::floor(row);
            Scalar r2 = std::ceil(row);
            Scalar c1 = std::floor(col);
            Scalar c2 = std::ceil(col);

            Scalar val11 = img(static_cast<Index>(r1), static_cast<Index>(c1), depth);
            Scalar val12 = img(static_cast<Index>(r1), static_cast<Index>(c2), depth);
            Scalar val21 = img(static_cast<Index>(r2), static_cast<Index>(c1), depth);
            Scalar val22 = img(static_cast<Index>(r2), static_cast<Index>(c2), depth);

            // calculate factors in x direction
            Scalar fac1 = 1;
            Scalar fac2 = 0;
            if(c2 != c1)
            {
                fac1 = (c2 - col) / (c2 - c1);
                fac2 = (col - c1) / (c2 - c1);
            }

            Scalar f1 = fac1 * val11 + fac2 * val12;
            Scalar f2 = fac1 * val21 + fac2 * val22;

            // calculate factors in y direction
            fac1 = 1;
            fac2 = 0;
            if(r2 != r1)
            {
                fac1 = (r2 - row) / (r2 - r1);
                fac2 = (row - r1) / (r2 - r1);
            }

            return static_cast<ScalarA>(fac1 * f1 + fac2 * f2);
        }
    };

    namespace image
    {
        /** Compute the bilinear interpolation at the given image coordinates.
          * @param img source image used to compute the interpolation values
          * @param row row index of the interpolated pixel
          * @param col column index of the interpolated pixel
          * @param depth depth index of the interpolated pixel
          * @return interpolated pixel value */
        template<typename Scalar, typename ScalarA>
        inline ScalarA bilinear(const Eigen::Tensor<ScalarA, 3> &img,
            const Scalar row,
            const Scalar col,
            const Index depth)
        {
            BilinearInterpolation<Scalar> interpolator;
            return interpolator(img, row, col, depth);
        }
    }
}

#endif
