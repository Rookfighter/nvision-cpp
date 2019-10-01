/* backward_differences_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef CVE_BACKWARD_DIFFERENCES_FILTER_H_
#define CVE_BACKWARD_DIFFERENCES_FILTER_H_

#include "cve/core/kernel.h"
#include "cve/filter/gradient_filter.h"

namespace cve
{
    /** Filter functor to compute backward differences gradients. */
    template<typename Scalar, typename BorderHandling=BorderReflect>
    class BackwardDifferencesFilter
    {
    private:
        BorderHandling handling_;
    public:
        BackwardDifferencesFilter()
            : handling_()
        { }

        void setBorderHandling(const BorderHandling &handling)
        {
            handling_ = handling;
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const GradientMode::X,
            Eigen::Tensor<ScalarB, 3> &grad) const
        {
            Eigen::Matrix<Scalar, 1, 3> kernel;
            kernel << -1, 1, 0;
            kernel::apply(img, grad, kernel, handling_);
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const GradientMode::Y,
            Eigen::Tensor<ScalarB, 3> &grad) const
        {
            Eigen::Matrix<Scalar, 3, 1> kernel;
            kernel << -1, 1, 0;
            kernel::apply(img, grad, kernel, handling_);
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
