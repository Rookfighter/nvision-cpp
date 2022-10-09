/* shi_tomasi_feature.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#ifndef NVISION_SHI_TOMASI_FEATURE_H_
#define NVISION_SHI_TOMASI_FEATURE_H_

#include <Eigen/Eigenvalues>
#include "nvision/src/feature/harris_feature_base.h"

namespace nvision
{
    /** Functor which computes the Shi-Tomasi feature score. */
    template<typename _Scalar>
    class ShiTomasiScore
    {
    public:
        using Scalar = _Scalar;
        using Matrix2 = Eigen::Matrix<Scalar, 2, 2>;
        using Vector2 = Eigen::Matrix<Scalar, 2, 1>;

        Scalar operator()(const Matrix2 &M) const
        {
            Vector2 eigvals = M.eigenvalues().real();
            return eigvals.minCoeff();
        }
    };

    template<typename Scalar,
             typename SmoothFilter=GaussFilter<Scalar, 3>,
             typename GradientFilter=SobelFilter<Scalar>>
     using ShiTomasiFeature = HarrisFeatureBase<Scalar, ShiTomasiScore<Scalar>, SmoothFilter, GradientFilter>;
}

#endif
