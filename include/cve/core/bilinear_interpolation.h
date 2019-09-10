/* bilinear_interpolation.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Sep 2019
 */

#ifndef CVE_BILINEAR_INTERPOLATION_H_
#define CVE_BILINEAR_INTERPOLATION_H_

#include <unsupported/Eigen/CXX11/Tensor>
#include "cve/core/math.h"
#include "cve/core/matrix.h"

namespace cve
{
    template<typename Scalar>
    class BilinearInterpolator
    {
    public:
        BilinearInterpolator()
        { }

        template<typename ScalarA>
        ScalarA operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const Scalar r,
            const Scalar c,
            const Index d) const
        {
            Scalar r1 = std::floor(r);
            Scalar r2 = std::ceil(r);
            Scalar c1 = std::floor(c);
            Scalar c2 = std::ceil(c);

            Scalar val11 = img(static_cast<Index>(r1), static_cast<Index>(c1), d);
            Scalar val12 = img(static_cast<Index>(r1), static_cast<Index>(c2), d);
            Scalar val21 = img(static_cast<Index>(r2), static_cast<Index>(c1), d);
            Scalar val22 = img(static_cast<Index>(r2), static_cast<Index>(c2), d);

            // calculate factors in x direction
            Scalar fac1 = 1;
            Scalar fac2 = 0;
            if(c2 != c1)
            {
                fac1 = (c2 - c) / (c2 - c1);
                fac2 = (c - c1) / (c2 - c1);
            }

            Scalar f1 = fac1 * val11 + fac2 * val12;
            Scalar f2 = fac1 * val21 + fac2 * val22;

            // calculate factors in y direction
            fac1 = 1;
            fac2 = 0;
            if(r2 != r1)
            {
                fac1 = (r2 - r) / (r2 - r1);
                fac2 = (r - r1) / (r2 - r1);
            }

            return static_cast<ScalarA>(fac1 * f1 + fac2 * f2);
        }
    };

    namespace image
    {
        template<typename Scalar, typename ScalarA>
        inline ScalarA bilinear(const Eigen::Tensor<ScalarA, 3> &img,
            const Scalar r,
            const Scalar c,
            const Index d)
        {
            BilinearInterpolator<Scalar> interpolator;
            return interpolator(img, r, c, d);
        }
    }
}

#endif
