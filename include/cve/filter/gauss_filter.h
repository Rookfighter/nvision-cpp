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
    template<typename Scalar, unsigned int Dim = 3>
    class GaussFilter
    {
    public:
        static_assert(Dim % 2 == 1, "GaussFilter dimension must be odd");
    private:
        Kernel<Scalar, Dim> kernel_;
        Scalar sigma_;

        void computeKernel()
        {
            Index offset = kernel_.dimension() / 2;
            for(Index i = 0; i < kernel_.matrix().cols(); ++i)
            {
                Scalar x = i - offset;
                for(Index j = 0; j < kernel_.matrix().rows(); ++j)
                {
                    Scalar y = j - offset;
                    // omit gauss normalization factor
                    // the kernel is normalized after the loop
                    kernel_.matrix()(j, i) = std::exp(-(x * x + y * y) /
                        (2 * sigma_ * sigma_));
                }
            }

            // normalize kernel such that sum of elements is one
            // if it is not normalized, the image becomes darker
            kernel_.matrix() /= kernel_.matrix().sum();
        }
    public:
        GaussFilter()
            : kernel_(), sigma_(1)
        {
            computeKernel();
        }

        GaussFilter(const Scalar sigma)
            : kernel_(), sigma_(sigma)
        {
            computeKernel();
        }

        void setSigma(const Scalar sigma)
        {
            sigma_ = sigma;
            computeKernel();
        }


        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            kernel_.apply<Image>(img, outImg);
        }
    };
}

#endif
