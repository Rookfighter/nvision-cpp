/* sobel_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 30 Apr 2019
 */

#ifndef CVE_SOBEL_FILTER_H_
#define CVE_SOBEL_FILTER_H_

#include "cve/core/kernel.h"
#include "cve/filter/gradient_filter.h"

namespace cve
{
    /** Filter class to compute Sobel gradients.
     *  The Sobel operator computes the first derivative of the image.
     *  @tparam Scalar value type of the underlying kernel*/
    template<typename Scalar>
    class SobelFilter
    {
    private:
        BorderHandling handling_;
    public:
        SobelFilter()
            : handling_(BorderHandling::Reflect)
        { }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const GradientMode::X,
            Eigen::Tensor<ScalarB, 3> &grad) const
        {
            Eigen::Matrix<Scalar, 1, 3> kernelX;
            Eigen::Matrix<Scalar, 3, 1> kernelY;

            kernelX << -1, 0, 1;
            kernelY << 1, 2, 1;

            Eigen::Tensor<ScalarB, 3> tmp;
            kernel::apply(img, tmp, kernelX, handling_);
            kernel::apply(tmp, grad, kernelY, handling_);
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const GradientMode::Y,
            Eigen::Tensor<ScalarB, 3> &grad) const
        {
            Eigen::Matrix<Scalar, 1, 3> kernelX;
            Eigen::Matrix<Scalar, 3, 1> kernelY;

            kernelX << 1, 2, 1;
            kernelY << -1, 0, 1;

            Eigen::Tensor<ScalarB, 3> tmp;
            kernel::apply(img, tmp, kernelX, handling_);
            kernel::apply(tmp, grad, kernelY, handling_);
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            Eigen::Tensor<ScalarB, 3> &gradX,
            Eigen::Tensor<ScalarB, 3> &gradY) const
        {
            operator()(img, GradientMode::X(), gradX);
            operator()(img, GradientMode::Y(), gradY);
        }
    };
}

#endif
