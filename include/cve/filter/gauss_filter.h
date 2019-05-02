/* gauss_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_GAUSS_FILTER_H_
#define CVE_GAUSS_FILTER_H_

#include "cve/kernel.h"

namespace cve
{
    /** Implements a Gaussian blur filter. */
    template<typename Scalar, unsigned int Dim = 3>
    class GaussFilter
    {
    public:
        static_assert(Dim % 2 == 1, "GaussFilter dimension must be odd");
    private:
        Scalar sigma_;
        BorderHandling handling_;

        Eigen::Matrix<Scalar, Dim, 1> kernelX_;
        Eigen::Matrix<Scalar, 1, Dim> kernelY_;

        template<typename Derived>
        void computeKernel(Eigen::MatrixBase<Derived> &kernel)
        {
            Index rowHalf = kernel.rows() / 2;
            Index colHalf = kernel.cols() / 2;

            for(Index c = 0; c < kernel.cols(); ++c)
            {
                Scalar x = c - colHalf;
                for(Index r = 0; r < kernel.rows(); ++r)
                {
                    Scalar y = r - rowHalf;
                    // omit gauss normalization factor
                    // the kernel is normalized after the loop
                    kernel(r, c) = std::exp(-(x * x + y * y) /
                        (2 * sigma_ * sigma_));
                }
            }

            // normalize kernel such that sum of elements is one
            // if it is not normalized, the image becomes darker
            kernel /= kernel.sum();
        }
    public:
        GaussFilter()
            : GaussFilter(1)
        {
        }

        GaussFilter(const Scalar sigma)
            : sigma_(), handling_(BorderHandling::Reflect), kernelX_(),
            kernelY_()
        {
            setSigma(sigma);
        }

        void setSigma(const Scalar sigma)
        {
            sigma_ = sigma;
            computeKernel(kernelX_);
            computeKernel(kernelY_);
        }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            Image tmpImg;
            kernel::apply(img, tmpImg, kernelX_, handling_);
            kernel::apply(tmpImg, outImg, kernelY_, handling_);
        }
    };
}

#endif
