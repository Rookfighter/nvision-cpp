/* scharr_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 02 May 2019
 */

#ifndef CVE_SCHARR_FILTER_H_
#define CVE_SCHARR_FILTER_H_

#include "cve/kernel.h"

namespace cve
{
    /** Filter class to compute Scharr gradients.
     *  @tparam Scalar value type of the underlying kernel */
    template<typename Scalar>
    class ScharrFilter
    {
    private:
        BorderHandling handling_;
    public:
        ScharrFilter()
            : handling_(BorderHandling::Reflect)
        {
        }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename ImageA, typename ImageB>
        void applyX(const ImageA &srcImg,
            ImageB &destImg) const
        {
            static_assert(ImageA::Depth == ImageB::Depth,
                "ImageA and ImageB must have same depth.");

            Eigen::Matrix<Scalar, 3, 3> kernel;

            kernel << 47, 0, -47,
                      162, 0, -162,
                      47, 0, -47;

            kernel::apply(srcImg, destImg, kernel, handling_);
        }

        template<typename ImageA, typename ImageB>
        void applyY(const ImageA &srcImg,
            ImageB &destImg) const
        {
            Eigen::Matrix<Scalar, 3, 3> kernel;

            kernel << 47, 162, 47,
                      0, 0, 0,
                      -47, -162, -47;

            kernel::apply(srcImg, destImg, kernel, handling_);
        }

        template<typename ImageA, typename ImageB>
        void apply(const ImageA &srcImg,
            ImageB &destImg) const
        {
            ImageB srcx;
            ImageB srcy;
            applyX(srcImg, srcx);
            applyY(srcImg, srcy);

            destImg = (srcx * srcx + srcy * srcy).sqrt();
        }
    };
}

#endif
