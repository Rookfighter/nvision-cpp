/* harris_features.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#ifndef CVE_HARRIS_FEATURES_H_
#define CVE_HARRIS_FEATURES_H_

#include "cve/feature/harris_features_base.h"

namespace cve
{
    /** Harris corner detection functor. */
    template<typename Scalar,
        typename SmoothFilter=GaussFilter<Scalar>,
        typename GradientFilter=SobelFilter<Scalar>>
    class HarrisFeatures : public HarrisFeaturesBase<Scalar, SmoothFilter, GradientFilter>
    {
    private:
        Scalar traceFactor_;

    protected:
        typedef Eigen::Matrix<Scalar, 2, 2> Matrix2;

        Scalar computeScore(const Matrix2 &M) const override
        {
            Scalar trace = M(0, 0) + M(1, 1);
            Scalar det = M(0, 0) * M(1, 1) - M(0, 1) * M(1, 0);
            return det - traceFactor_ * trace * trace;
        }

    public:
        HarrisFeatures()
            : HarrisFeatures(0.04, 0.01, 10, 0)
        {

        }

        HarrisFeatures(const Scalar traceFactor,
            const Scalar qualityLevel,
            const size_t minDistance,
            const size_t maxCorners)
            : HarrisFeaturesBase<Scalar, SmoothFilter, GradientFilter>(
                qualityLevel, minDistance, maxCorners),
                traceFactor_(traceFactor)
        {

        }

        ~HarrisFeatures()
        {

        }


        /** Sets the weight of the trace in the harris corner score.
          * The value is typically in the range of [0.05,0.15].
          * @param traceFactor factor of the trace */
        void setTraceFactor(const Scalar traceFactor)
        {
            traceFactor_ = traceFactor;
        }
    };
}

#endif
