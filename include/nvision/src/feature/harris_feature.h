/* harris_feature.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#ifndef NVISION_HARRIS_FEATURE_H_
#define NVISION_HARRIS_FEATURE_H_

#include "nvision/src/feature/harris_feature_base.h"

namespace nvision
{
    /** Functor which computes the Harris feature score. */
    template<typename _Scalar>
    class HarrisScore
    {
    public:
        using Scalar = _Scalar;
        using Matrix2 = Eigen::Matrix<Scalar, 2, 2>;

        HarrisScore() = default;

        HarrisScore(const Scalar traceFactor)
            : traceFactor_(traceFactor)
        { }

        Scalar operator()(const Matrix2 &M) const
        {
            Scalar trace = M(0, 0) + M(1, 1);
            Scalar det = M(0, 0) * M(1, 1) - M(0, 1) * M(1, 0);
            return det - traceFactor_ * trace * trace;
        }

    private:
        Scalar traceFactor_ = static_cast<Scalar>(0.04);
    };

    template<typename Scalar,
             typename SmoothFilter=GaussFilter<Scalar, 3>,
             typename GradientFilter=SobelFilter<Scalar>>
     using HarrisFeature = HarrisFeatureBase<Scalar, HarrisScore<Scalar>, SmoothFilter, GradientFilter>;
}

#endif
