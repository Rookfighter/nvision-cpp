/* laplace_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef CVE_LAPLACE_FILTER_H_
#define CVE_LAPLACE_FILTER_H_

#include "cve/kernel.h"

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

        template<typename ImageA, typename ImageB>
        void apply(const ImageA &srcImg, ImageB &destImg) const
        {
            static_assert(ImageA::Depth == ImageB::Depth,
                "ImageA and ImageB must have same depth.");

            Eigen::Matrix<Scalar, 3, 3> kernel;

            kernel << 0,  1, 0,
                      1, -4, 1,
                      0,  1, 0;

            kernel::apply(srcImg, destImg, kernel, handling_);
        }
    };
}

#endif
