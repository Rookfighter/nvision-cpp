/* lucas_kanade_flow.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef NVISION_LUCAS_KANADE_FLOW_H_
#define NVISION_LUCAS_KANADE_FLOW_H_

#include <Eigen/Dense>
#include "nvision/src/core/math.h"
#include "nvision/src/optflow/flow_field.h"
#include "nvision/src/filter/gauss_filter.h"
#include "nvision/src/filter/sobel_filter.h"

namespace nvision
{

    /** Implements calculation of optical flow according to the Lucas Kanade method. */
    template<typename _Scalar,
             typename _SmoothFilter = GaussFilter<_Scalar, 3>,
             typename _GradientFilter = SobelFilter<_Scalar>>
    class LucasKanadeFlow
    {
    public:
        using Scalar = _Scalar;
        using SmoothFilter = _SmoothFilter;
        using GradientFilter = _GradientFilter;

        LucasKanadeFlow() = default;

        void setSmoothFilter(const SmoothFilter &filter)
        {
            _smooth = filter;
        }

        void setGradientFilter(const GradientFilter &filter)
        {
            _gradient = filter;
        }

        template<typename DerivedA, typename DerivedB>
        void operator()(const ImageBase<DerivedA> &imgA,
                        const ImageBase<DerivedB> &imgB,
                        FlowField<Scalar> &flowField) const
        {
            static_assert(IsImage<ImageBase<DerivedA>>::value, "image A must be image type");
            static_assert(IsImage<ImageBase<DerivedB>>::value, "image B must be image type");
            static_assert(std::is_same<
                typename ImageBase<DerivedA>::Scalar::ColorSpace,
                typename ImageBase<DerivedB>::Scalar::ColorSpace>::value,
                "images must use same color space");

            using ColorSpace = typename ImageBase<DerivedA>::Scalar::ColorSpace;
            assert(imgA.rows() == imgB.rows());
            assert(imgA.cols() == imgB.cols());

            using Matrix2 = Eigen::Matrix<Scalar, 2, 2>;
            using Vector2 = Eigen::Matrix<Scalar, 2, 1>;

            Image<ColorSpace> gradX = _gradient(imgA, GradientMode::X());
            Image<ColorSpace> gradY = _gradient(imgA, GradientMode::Y());
            Image<ColorSpace> gradT = imgB - imgA;

            Image<ColorSpace> tmp = gradX * gradX;
            Image<ColorSpace> gradXX = _smooth(tmp);

            tmp = gradY * gradY;
            Image<ColorSpace> gradYY = _smooth(tmp);

            tmp = gradX * gradY;
            Image<ColorSpace> gradXY = _smooth(tmp);

            tmp = gradX * gradT;
            Image<ColorSpace> gradXT = _smooth(tmp);

            tmp = gradY * gradT;
            Image<ColorSpace> gradYT = _smooth(tmp);

            flowField.resize(imgA.rows(), imgA.cols());

            for(Index c = 0; c < imgA.cols(); ++c)
            {
                for(Index r = 0; r < imgA.rows(); ++r)
                {
                    flowField(r, c).setZero();
                    const auto &xx = gradXX(r, c);
                    const auto &xy = gradXY(r, c);
                    const auto &yy = gradYY(r, c);
                    const auto &xt = gradXT(r, c);
                    const auto &yt = gradYT(r, c);

                    for(Index d = 0; d < ColorSpace::Dimension; ++d)
                    {
                        Matrix2 A;
                        A << xx[d], xy[d],
                             xy[d], yy[d];
                        Vector2 b;
                        b << -xt[d], -yt[d];

                        flowField(r, c) += A.inverse() * b;
                    }

                    flowField(r, c) /= static_cast<Scalar>(ColorSpace::Dimension);
                    if(flowField(r, c).hasNaN())
                        flowField(r, c).setZero();
                }
            }
        }
    private:
        SmoothFilter _smooth = {};
        GradientFilter _gradient = {};
    };
}

#endif
