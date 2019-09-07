/* scharr_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 02 May 2019
 */

#ifndef CVE_SCHARR_FILTER_H_
#define CVE_SCHARR_FILTER_H_

#include "cve/core/kernel.h"
#include "cve/filter/gradient_filter.h"

namespace cve
{
    /** Filter class to compute Scharr gradients.
     *  The Scharr operator computes the first derivative of the image.
     *  @tparam Scalar value type of the underlying kernel */
    template<typename Scalar>
    class ScharrFilter
    {
    private:
        BorderHandling handling_;
    public:
        ScharrFilter()
            : handling_(BorderHandling::Reflect)
        { }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            const GradientMode::X,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Matrix<Scalar, 3, 3> kernel;
            kernel << -47, 0, 47,
                      -162, 0, 162,
                      -47, 0, 47;
            kernel::apply(srcImg, destImg, kernel, handling_);
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            const GradientMode::Y,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Matrix<Scalar, 3, 3> kernel;
            kernel << -47, -162, -47,
                      0, 0, 0,
                      47, 162, 47;
            kernel::apply(srcImg, destImg, kernel, handling_);
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
