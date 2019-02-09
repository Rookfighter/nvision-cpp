/* gaussian_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_GAUSSIAN_FILTER_H_
#define CVE_GAUSSIAN_FILTER_H_

#include "cve/kernel.h"

namespace cve
{
    template<typename Scalar, int Dim = 3>
    class GaussianFilter
    {
    private:
        Kernel<Scalar, Dim, Dim> kernel_;
    public:
        GaussianFilter()
        {
            #if Dim == 3
            kernel_.matrix() << 1, 2, 1,
                2, 4, 2,
                1, 2, 1;
            kernel_.matrix() /= 16;
            #elif Dim == 5
            kernel_.matrix() <<
                1, 4, 6, 4, 1,
                4, 16, 24, 16, 4,
                6 24, 36, 24, 6,
                4, 16, 24, 16, 4,
                1, 4, 6, 4, 1;
            kernel_.matrix() /= 256;
            #else
            #error Only gaussian filter with dim = 3 or 5 supported
            #endif
        }

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            kernel_.apply<Image>(img, outImg);
        }
    };
}

#endif
