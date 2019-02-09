/* laplacian_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_LAPLACIAN_FILTER_H_
#define CVE_LAPLACIAN_FILTER_H_

#include "cve/kernel.h"

namespace cve
{
    template<typename Scalar>
    class LaplacianFilter
    {
    private:
        Kernel<Scalar, 3, 3> kernel_;
    public:
        LaplacianFilter()
        {
            kernel_.matrix() <<
                0, 1, 0,
                1, -4, 1,
                0, 1, 1;
        }

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            kernel_.apply<Image>(img, outImg);
        }
    };
}

#endif
