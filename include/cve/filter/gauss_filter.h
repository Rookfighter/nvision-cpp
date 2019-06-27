/* gauss_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_GAUSS_FILTER_H_
#define CVE_GAUSS_FILTER_H_

#include "cve/core/kernel.h"

namespace cve
{
    /** Filter class to apply Gaussian blur.
     *  A kernel size which is greater than ceil(6 * sigma) brings no advantage
     *  in precision.
     *  @tparam Scalar value type of the underlying kernel
     *  @tparam Dim size of the underlying kernel */
    template<typename Scalar, unsigned int Dim = 3>
    class GaussFilter
    {
    public:
        static_assert(Dim > 1, "GaussFilter dimension must be greater than one");
        static_assert(Dim % 2 == 1, "GaussFilter dimension must be odd");
    private:
        Scalar sigma_;
        BorderHandling handling_;

        Eigen::Matrix<Scalar, Dim, 1> kernel_;

        void computeKernel()
        {
            Index rowHalf = kernel_.rows() / 2;

            for(Index i = 0; i < rowHalf + 1; ++i)
            {
                Index idxA = rowHalf - i;
                Index idxB = rowHalf + i;
                // omit gauss normalization factor
                // the kernel is normalized after the loop
                Scalar value = std::exp(-(i * i) /
                    (2 * sigma_ * sigma_));
                kernel_(idxA) = value;
                kernel_(idxB) = value;
            }

            // normalize kernel such that sum of elements is one
            // if it is not normalized, the image becomes darker
            kernel_ /= kernel_.sum();
        }
    public:
        GaussFilter()
            : GaussFilter(1)
        {
        }

        GaussFilter(const Scalar sigma)
            : sigma_(), handling_(BorderHandling::Reflect), kernel_()
        {
            setSigma(sigma);
        }

        void setSigma(const Scalar sigma)
        {
            sigma_ = sigma;
            computeKernel();
        }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename ScalarA, typename ScalarB>
        void apply(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Tensor<ScalarB, 3> tmpImg;
            kernel::apply(srcImg, tmpImg, kernel_, handling_);
            kernel::apply(tmpImg, destImg, kernel_.transpose(), handling_);
        }

        template<typename ScalarA>
        void apply(Eigen::Tensor<ScalarA, 3> &img) const
        {
            Eigen::Tensor<ScalarA, 3> tmp;
            apply(img, tmp);
            img = tmp;
        }
    };
}

#endif
