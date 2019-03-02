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
    template<typename Scalar, int Dim = 3>
    class GaussFilter
    {
    private:
        typedef typename Kernel<Scalar, Dim, Dim>::Index Index;

        Kernel<Scalar, Dim, Dim> kernel_;
        Scalar sigma_;

        void computeKernel()
        {
            Index offset = kernel_.dimension() / 2;
            for(Index i = 0; i < kernel_.matrix().cols(); ++i)
            {
                Index x = i - offset;
                for(Index j = 0; j < kernel_.matrix().rows(); ++j)
                {
                    Index y = j - offset;
                    kernel_.matrix()(j, i) = std::exp(-(x * x + y * y) / (2 * sigma_ * sigma_));
                }
            }

            const Scalar sqrttwopi = 2.5066282746310005;
            kernel_.matrix() /= sigma_ * sqrttwopi;
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
