/* harris_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#ifndef CVE_HARRIS_DETECTOR_H_
#define CVE_HARRIS_DETECTOR_H_

#include "cve/feature/harris_base_detector.h"

namespace cve
{
    /** Class for harris corner detection. */
    template<typename Scalar,
        typename SmoothFilter=GaussFilter<Scalar, 5>,
        typename GradientFilter=SobelFilter<Scalar>>
    class HarrisDetector : public HarrisBaseDetector<Scalar, SmoothFilter, GradientFilter>
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
        HarrisDetector()
            : HarrisDetector(0.04, 0.01, 10, 0)
        {

        }

        HarrisDetector(const Scalar traceFactor,
            const Scalar qualityLevel,
            const size_t minDistance,
            const size_t maxCorners)
            : HarrisBaseDetector<Scalar, SmoothFilter, GradientFilter>(
                qualityLevel, minDistance, maxCorners),
                traceFactor_(traceFactor)
        {

        }

        ~HarrisDetector()
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
