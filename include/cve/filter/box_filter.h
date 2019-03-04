/* box_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_BOX_FILTER_H_
#define CVE_BOX_FILTER_H_

#include "cve/kernel.h"

namespace cve
{
    template<typename Scalar, int Dim = 3>
    class BoxFilter
    {
    private:
        typedef typename Kernel<Scalar, Dim>::Index Index;

        Kernel<Scalar, Dim> kernel_;
        Scalar sigma_;
        size_t iterations_;

        void computeKernel()
        {
            kernel_.matrix().setConstant(1 / (2 * sigma_));
            kernel_.matrix() /= kernel_.matrix().sum();

            Kernel<Scalar, Dim> kernelTmp;
            for(size_t i = 0; i < iterations_; ++i)
            {
                kernel_.apply(kernel_, kernelTmp);
                kernel_ = kernelTmp;
            }
        }
    public:
        BoxFilter()
            : BoxFilter(1, 1)
        { }

        BoxFilter(const Scalar sigma)
            : BoxFilter(sigma, 1)
        { }

        BoxFilter(const Scalar sigma, const size_t iterations)
            : kernel_(), sigma_(sigma), iterations_(iterations)
        {
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
