/* shi_tomasi_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#ifndef CVE_SHI_TOMASI_DETECTOR_H_
#define CVE_SHI_TOMASI_DETECTOR_H_

#include <Eigen/Eigenvalues>
#include "cve/feature/harris_base_detector.h"

namespace cve
{
    /** Class for Shi-Tomasi corner detection. */
    template<typename Scalar,
        typename SmoothFilter=GaussFilter<Scalar>,
        typename GradientFilter=SobelFilter<Scalar>>
    class ShiTomasiDetector : public HarrisBaseDetector<Scalar, SmoothFilter, GradientFilter>
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;;
    protected:
        typedef Eigen::Matrix<Scalar, 2, 2> Matrix2;
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;

        Scalar computeScore(const Matrix2 &M) const override
        {
            Vector2 eigvals = M.eigenvalues().real();
            return eigvals.minCoeff();
        }

    public:
        ShiTomasiDetector()
            : ShiTomasiDetector(0.01, 10, 0)
        {

        }

        ShiTomasiDetector(const Scalar qualityLevel,
            const size_t minDistance,
            const size_t maxCorners)
            : HarrisBaseDetector<Scalar, SmoothFilter, GradientFilter>(
                qualityLevel, minDistance, maxCorners)
        {

        }

        ~ShiTomasiDetector()
        {

        }
    };
}

#endif
