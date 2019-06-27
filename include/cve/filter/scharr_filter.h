/* scharr_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 02 May 2019
 */

#ifndef CVE_SCHARR_FILTER_H_
#define CVE_SCHARR_FILTER_H_

#include "cve/core/kernel.h"

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
        {
        }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename ScalarA, typename ScalarB>
        void applyX(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Matrix<Scalar, 3, 3> kernel;

            kernel << 47, 0, -47,
                      162, 0, -162,
                      47, 0, -47;

            kernel::apply(srcImg, destImg, kernel, handling_);
        }

        template<typename ScalarA, typename ScalarB>
        void applyY(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Matrix<Scalar, 3, 3> kernel;

            kernel << 47, 162, 47,
                      0, 0, 0,
                      -47, -162, -47;

            kernel::apply(srcImg, destImg, kernel, handling_);
        }

        template<typename ScalarA, typename ScalarB>
        void apply(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Tensor<ScalarB, 3> gradX;
            Eigen::Tensor<ScalarB, 3> gradY;
            applyX(srcImg, gradX);
            applyY(srcImg, gradY);

            destImg = (gradX * gradX + gradY * gradY).sqrt();
        }
    };
}

#endif
