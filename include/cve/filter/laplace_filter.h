/* laplace_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef CVE_LAPLACE_FILTER_H_
#define CVE_LAPLACE_FILTER_H_

#include "cve/core/kernel.h"

namespace cve
{
    /** Filter class to compute Laplacian gradients.
     *  The Laplace operator computes the second derivative of the image.
     *  @tparam Scalar value type of the underlying kernel */
    template<typename Scalar>
    class LaplaceFilter
    {
    private:
        BorderHandling handling_;

    public:
        LaplaceFilter()
            : handling_(BorderHandling::Reflect)
        {
        }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Matrix<Scalar, 3, 3> kernel;

            kernel << 0,  1, 0,
                      1, -4, 1,
                      0,  1, 0;

            kernel::apply(srcImg, destImg, kernel, handling_);
        }

        template<typename ScalarA>
        void operator()(Eigen::Tensor<ScalarA, 3> &img) const
        {
            Eigen::Tensor<ScalarA, 3> tmp;
            apply(img, tmp);
            img = tmp;
        }
    };
}

#endif
