/* horn_schunck_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef CVE_HORN_SCHUNCK_DETECTOR_H_
#define CVE_HORN_SCHUNCK_DETECTOR_H_

#include "cve/filter/gauss_filter.h"
#include "cve/filter/sobel_filter.h"

namespace cve
{
    template<typename Scalar,
        typename SmoothFilter = GaussFilter<Scalar>,
        typename GradientFilter = SobelFilter<Scalar>>
    class HornSchunckDetector
    {
    private:
        Scalar alpha_;
        Index maxIt_;
        SmoothFilter smoothFilter_;
        GradientFilter gradientFilter_;
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;

        HornSchunckDetector()
            : HornSchunckDetector(1, 100)
        {
        }

        HornSchunckDetector(const Scalar alpha, const Index maxIt)
            : alpha_(alpha), maxIt_(maxIt), smoothFilter_(), gradientFilter_()
        {
        }

        void setRegularizationConstant(const Scalar alpha)
        {
            alpha = alpha_;
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
            Eigen::Tensor<Scalar, 3> gradXX;
            Eigen::Tensor<Scalar, 3> gradYY;

            gradientFilter_(imgA, gradX, gradY);
            gradT = imgB.template cast<Scalar>() - imgA.template cast<Scalar>();
            gradXX = gradX * gradX;
            gradYY = gradY * gradY;

            Eigen::Tensor<Scalar, 3> flowU(height, width, 1);
            Eigen::Tensor<Scalar, 3> flowV(height, width, 1);
            Eigen::Tensor<Scalar, 3> flowUAvg(height, width, 1);
            Eigen::Tensor<Scalar, 3> flowVAvg(height, width, 1);
            Eigen::Tensor<Scalar, 3> tmp(height, width, 1);

            flowU.setZero();
            flowV.setZero();
            Scalar lambda = 4 * alpha_ * alpha_;

            for(Index i = 0; i < maxIt_; ++i)
            {
                smoothFilter_(flowU, flowUAvg);
                smoothFilter_(flowV, flowVAvg);
                tmp = (gradX * flowUAvg + gradY * flowVAvg + gradT) /
                    (gradXX.constant(lambda) + gradXX + gradYY);
                flowU = flowUAvg - gradX * tmp;
                flowV = flowVAvg - gradY * tmp;
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
