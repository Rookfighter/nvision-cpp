/* robust_flow_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 06 Sep 2019
 */

#ifndef CVE_ROBUST_FLOW_DETECTOR_H_
#define CVE_ROBUST_FLOW_DETECTOR_H_

#include "cve/filter/box_filter.h"
#include "cve/filter/central_differences_filter.h"
#include "cve/filter/forward_differences_filter.h"
#include "cve/filter/sobel_filter.h"
#include "cve/core/penalizer_functors.h"

namespace cve
{
    template<typename Scalar,
        typename Penalizer = TotalVariationPenalizer<Scalar>,
        typename GradientFilter = CentralDifferencesFilter<Scalar>>
    class RobustFlowDetector
    {
    private:
        Index maxIt_;
        Scalar alpha_;
        GradientFilter gradientFilter_;
        Penalizer penalizer_;
    public:
        RobustFlowDetector()
            : RobustFlowDetector(50, 20)
        { }

        RobustFlowDetector(const Index iterations, const Scalar alpha)
            : maxIt_(iterations), alpha_(alpha),
            gradientFilter_(), penalizer_()
        { }

        void setMaxIterations(const Index iterations)
        {
            maxIt_ = iterations;
        }

        void setAlpha(const Scalar alpha)
        {
            alpha_ = alpha;
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
        void operator()(const Eigen::Tensor<ScalarA, 3> &imgA,
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
            Eigen::Tensor<Scalar, 3> flowUM(height, width, 1);
            Eigen::Tensor<Scalar, 3> flowVM(height, width, 1);

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

            Eigen::Tensor<Scalar, 3> stepSizes = 1 / (gradAX * gradAX + gradAY * gradAY + alpha_ * alpha_);

            Eigen::Matrix<Scalar, 3, 3> kernel;
            kernel << 0, 1, 0,
                      1, 0, 1,
                      0, 1, 0;
            kernel /= 4;
            for(Index i = 0; i < maxIt_; ++i)
            {
                kernel::apply(flowU, flowUM, kernel, BorderHandling::Reflect);
                kernel::apply(flowV, flowVM, kernel, BorderHandling::Reflect);

                dataTerm = gradAX * flowUM + gradAY * flowVM + gradT;
                dataTermP = (dataTerm * dataTerm).unaryExpr(penalizer_);

                gradTermU = gradAXX * flowUM + gradAXY * flowVM + gradXT;
                gradTermV = gradAYX * flowUM + gradAYY * flowVM + gradYT;
                gradTerm = (gradTermU * gradTermU + gradTermV * gradTermV).unaryExpr(penalizer_);

                flowU = flowUM - stepSizes * (dataTermP * dataTerm * gradAX
                    + gradTerm * (gradTermU * gradAXX + gradTermV * gradAXY));
                flowV = flowVM - stepSizes * (dataTermP * dataTerm * gradAY
                    + gradTerm * (gradTermU * gradAYX + gradTermV * gradAYY));
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
