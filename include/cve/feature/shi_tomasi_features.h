/* shi_tomasi_features.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#ifndef CVE_SHI_TOMASI_FEATURES_H_
#define CVE_SHI_TOMASI_FEATURES_H_

#include <Eigen/Eigenvalues>
#include "cve/feature/harris_features_base.h"

namespace cve
{
    /** Shi-Tomasi corner detection functor. */
    template<typename Scalar,
        typename SmoothFilter=GaussFilter<Scalar>,
        typename GradientFilter=SobelFilter<Scalar>>
    class ShiTomasiFeatures : public HarrisFeaturesBase<Scalar, SmoothFilter, GradientFilter>
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
        ShiTomasiFeatures()
            : ShiTomasiFeatures(0.01, 10, 0)
        {

        }

        ShiTomasiFeatures(const Scalar qualityLevel,
            const size_t minDistance,
            const size_t maxCorners)
            : HarrisFeaturesBase<Scalar, SmoothFilter, GradientFilter>(
                qualityLevel, minDistance, maxCorners)
        {

        }

        ~ShiTomasiFeatures()
        {

        }
    };
}

#endif
