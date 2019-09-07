/* horn_schunck_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef CVE_HORN_SCHUNCK_DETECTOR_H_
#define CVE_HORN_SCHUNCK_DETECTOR_H_

#include "cve/filter/box_filter.h"
#include "cve/filter/central_differences_filter.h"

namespace cve
{
    /** Optical flow detector which uses the Horn-Schunk method.
      * It basically solves the global energy functional
      *
      * Integral( (Ix * u + Iy * v + It)^2 + alpha^2 (ux^2 + uy^2 + vx^2 + vy^2) )
      *
      * Using Euler-Lagrange equations:
      *
      * Ix * (Ix * u + Iy * v + It) - alpha^2 lap(u) = 0
      * Iy * (Ix * u + Iy * v + It) - alpha^2 lap(v) = 0
      */
    template<typename Scalar,
        typename SmoothFilter = BoxFilter<Scalar>,
        typename GradientFilter = CentralDifferencesFilter<Scalar>>
    class HornSchunckDetector
    {
    private:
        Index iterations_;
        Scalar alpha_;
        SmoothFilter smoothFilter_;
        GradientFilter gradientFilter_;
    public:
        HornSchunckDetector()
            : HornSchunckDetector(100, 15)
        { }

        HornSchunckDetector(const Index iterations, const Scalar alpha)
            : iterations_(iterations), alpha_(alpha),
            smoothFilter_(), gradientFilter_()
        { }

        /** Sets the regularization constant for the Horn-Schunck method.
          * Higher values make the result smoother.
          * @param alpha regularization constant */
        void setRegularizationConstant(const Scalar alpha)
        {
            alpha_ = alpha;
        }

        /** Set the number of iterations to the solve the equation system.
          * @param iterations number of iterations for the solver */
        void setIterations(const Index iterations)
        {
            iterations_ = iterations;
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

            Eigen::Tensor<Scalar, 3> gradX;
            Eigen::Tensor<Scalar, 3> gradY;
            Eigen::Tensor<Scalar, 3> gradT;

            gradientFilter_(imgA, gradX, gradY);
            gradT = imgB.template cast<Scalar>() - imgA.template cast<Scalar>();

            Eigen::Tensor<Scalar, 3> flowU(height, width, 1);
            Eigen::Tensor<Scalar, 3> flowV(height, width, 1);
            Eigen::Tensor<Scalar, 3> dataTerm;

            Eigen::Tensor<Scalar, 3> stepSizes = gradX.constant(1) / (gradX * gradX + gradY * gradY + 4 * alpha_ * alpha_);

            flowU.setZero();
            flowV.setZero();
            for(Index i = 0; i < iterations_; ++i)
            {
                smoothFilter_(flowU);
                smoothFilter_(flowV);
                dataTerm = gradX * flowU + gradY * flowV + gradT;
                flowU -= stepSizes * dataTerm * gradX;
                flowV -= stepSizes * dataTerm * gradY;
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
