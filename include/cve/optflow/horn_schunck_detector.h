/* horn_schunck_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef CVE_HORN_SCHUNCK_DETECTOR_H_
#define CVE_HORN_SCHUNCK_DETECTOR_H_

#include "cve/filter/gauss_filter.h"
#include "cve/filter/central_differences_filter.h"

namespace cve
{
    template<typename Scalar,
        typename SmoothFilter = GaussFilter<Scalar>,
        typename GradientFilter = CentralDifferencesFilter<Scalar>>
    class HornSchunckDetector
    {
    private:
        Index maxIt_;
        Scalar alpha_;
        SmoothFilter smoothFilter_;
        GradientFilter gradientFilter_;
    public:
        HornSchunckDetector()
            : HornSchunckDetector(10, 2)
        { }

        HornSchunckDetector(const Index iterations, const Scalar alpha)
            : maxIt_(iterations), alpha_(alpha),
            smoothFilter_(), gradientFilter_()
        {
        }

        void setAlpha(const Scalar alpha)
        {
            alpha_ = alpha;
        }

        void setMaxIterations(const Index iterations)
        {
            maxIt_ = iterations;
        }

        void setSmoothFilter(const SmoothFilter &filter)
        {
            smoothFilter_ = filter;
        }

        void setGradientFilter(const GradientFilter &filter)
        {
            gradientFilter_ = filter;
        }

        template<typename ScalarA>
        void apply(const Eigen::Tensor<ScalarA, 3> &imgA,
            const Eigen::Tensor<ScalarA, 3> &imgB,
            Eigen::Tensor<Scalar, 3> &flowImg) const
        {
            assert(imgA.dimension(2) == 1);
            assert(imgA.dimension(0) == imgB.dimension(0));
            assert(imgA.dimension(1) == imgB.dimension(1));
            assert(imgA.dimension(2) == imgB.dimension(2));

            Index height = imgA.dimension(0);
            Index width = imgA.dimension(1);

            Eigen::Tensor<Scalar, 3> gradX;
            Eigen::Tensor<Scalar, 3> gradY;
            Eigen::Tensor<Scalar, 3> gradT;

            gradientFilter_(imgA, gradX, gradY);
            gradT = imgB.template cast<Scalar>() - imgA.template cast<Scalar>();

            Eigen::Tensor<Scalar, 3> flowU(height, width, 1);
            Eigen::Tensor<Scalar, 3> flowV(height, width, 1);
            Eigen::Tensor<Scalar, 3> step(height, width, 1);

            flowU.setZero();
            flowV.setZero();

            Scalar lambda = 4 * alpha_ * alpha_;

            Eigen::Tensor<Scalar, 3> stepSizes = 1 / (gradX * gradX + gradY * gradY + lambda);

            for(Index i = 0; i < maxIt_; ++i)
            {
                step = gradX * flowU + gradY * flowV + gradT;
                flowU -= stepSizes * gradX * step;
                flowV -= stepSizes * gradY * step;
                smoothFilter_(flowU);
                smoothFilter_(flowV);
            }

            flowImg.resize(height, width, 2);
            for(Index c = 0; c < width; ++c)
            {
                for(Index r = 0; r < height; ++r)
                {
                    flowImg(r, c, 0) = flowU(r, c, 0);
                    flowImg(r, c, 1) = flowV(r, c, 0);
                }
            }
        }
    };
}

#endif
