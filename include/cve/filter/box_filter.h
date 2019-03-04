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

        void computeKernel()
        {
            kernel_.matrix().setConstant(1 / (2 * sigma_));
            kernel_.matrix() /= kernel_.matrix().sum();
        }
    public:
        BoxFilter()
            : kernel_(), sigma_(1)
        {
            computeKernel();
        }

        BoxFilter(const Scalar sigma)
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
