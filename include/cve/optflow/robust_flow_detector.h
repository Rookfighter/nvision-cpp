/* robust_flow_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 06 Sep 2019
 */

#ifndef CVE_ROBUST_FLOW_DETECTOR_H_
#define CVE_ROBUST_FLOW_DETECTOR_H_

#include "cve/filter/gauss_filter.h"
#include "cve/filter/central_differences_filter.h"
#include "cve/core/penalizer_functors.h"

namespace cve
{
    template<typename Scalar,
        typename Penalizer = TotalVariationPenalizer<Scalar>,
        typename SmoothFilter = GaussFilter<Scalar>,
        typename GradientFilter = CentralDifferencesFilter<Scalar>>
    class RobustFlowDetector
    {
    private:
        Index maxIt_;
        Scalar alpha_;
        SmoothFilter smoothFilter_;
        GradientFilter gradientFilter_;
        Penalizer penalizer_;
    public:
        RobustFlowDetector()
            : RobustFlowDetector(20, 1)
        { }

        RobustFlowDetector(const Index iterations, const Scalar alpha)
            : maxIt_(iterations), alpha_(alpha),
            smoothFilter_(), gradientFilter_(), penalizer_()
        { }

        void setMaxIterations(const Index iterations)
        {
            maxIt_ = iterations;
        }

        void setAlpha(const Scalar alpha)
        {
            alpha_ = alpha;
        }

        void setSmoothFilter(const SmoothFilter &filter)
        {
            smoothFilter_ = filter;
        }

        void setGradientFilter(const GradientFilter &filter)
        {
            gradientFilter_ = filter;
        }

        void setPenalizer(const Penalizer &penalizer)
        {
            penalizer_ = penalizer;
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

            Eigen::Tensor<Scalar, 3> gradAX;
            Eigen::Tensor<Scalar, 3> gradAY;
            Eigen::Tensor<Scalar, 3> gradBX;
            Eigen::Tensor<Scalar, 3> gradBY;
            Eigen::Tensor<Scalar, 3> gradAXX;
            Eigen::Tensor<Scalar, 3> gradAXY;
            Eigen::Tensor<Scalar, 3> gradAYX;
            Eigen::Tensor<Scalar, 3> gradAYY;
            Eigen::Tensor<Scalar, 3> gradT;
            Eigen::Tensor<Scalar, 3> gradXT;
            Eigen::Tensor<Scalar, 3> gradYT;

            Eigen::Tensor<Scalar, 3> flowU(height, width, 1);
            Eigen::Tensor<Scalar, 3> flowV(height, width, 1);

            Eigen::Tensor<Scalar, 3> dataTerm;
            Eigen::Tensor<Scalar, 3> dataTermP;
            Eigen::Tensor<Scalar, 3> gradTermU;
            Eigen::Tensor<Scalar, 3> gradTermV;
            Eigen::Tensor<Scalar, 3> gradTerm;

            gradientFilter_(imgA, gradAX, gradAY);
            gradientFilter_(imgB, gradBX, gradBY);
            gradientFilter_(gradAX, gradAXX, gradAXY);
            gradientFilter_(gradAY, gradAYX, gradAYY);

            gradT = imgB.template cast<Scalar>() - imgA.template cast<Scalar>();
            gradXT = gradBX - gradAX;
            gradYT = gradBY - gradAY;

            flowU.setZero();
            flowV.setZero();

            Scalar lambda = 4 * alpha_ * alpha_;
            Eigen::Tensor<Scalar, 3> stepSizes = 1 / (gradAX * gradAX + gradAY * gradAY + lambda);

            for(Index i = 0; i < maxIt_; ++i)
            {
                dataTerm = gradAX * flowU + gradAY * flowV + gradT;
                dataTermP = (dataTerm * dataTerm).unaryExpr(penalizer_);

                gradTermU = gradAXX * flowU + gradAXY * flowV + gradXT;
                gradTermV = gradAYX * flowU + gradAYY * flowV + gradYT;
                gradTerm = (gradTermU * gradTermU + gradTermV * gradTermV).unaryExpr(penalizer_);

                flowU -= stepSizes * (dataTermP * dataTerm * gradAX
                    + gradTerm * (gradTermU * gradAXX + gradTermV * gradAXY));
                flowV -= stepSizes * (dataTermP * dataTerm * gradAY
                    + gradTerm * (gradTermU * gradAYX + gradTermV * gradAYY));

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
