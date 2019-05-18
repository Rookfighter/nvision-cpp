/* derivative_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef CVE_DERIVATIVE_FILTER_H_
#define CVE_DERIVATIVE_FILTER_H_

#include "cve/kernel.h"

namespace cve
{
    /** Filter class to compute central differences gradients.
     *  @tparam Scalar value type of the underlying kernel*/
    template<typename Scalar>
    class DerivativeFilter
    {
    private:
        BorderHandling handling_;
    public:
        DerivativeFilter()
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

            Eigen::Matrix<Scalar, 1, 3> kernel;
            kernel << -1, 0, 1;

            kernel::apply(srcImg, destImg, kernel, handling_);
        }

        template<typename ImageA, typename ImageB>
        void applyY(const ImageA &srcImg,
            ImageB &destImg) const
        {
            static_assert(ImageA::Depth == ImageB::Depth,
                "ImageA and ImageB must have same depth.");

            Eigen::Matrix<Scalar, 3, 1> kernel;
            kernel << -1, 0, 1;

            kernel::apply(srcImg, destImg, kernel, handling_);
        }

        template<typename ImageA, typename ImageB>
        void apply(const ImageA &srcImg,
            ImageB &destImg) const
        {
            static_assert(ImageA::Depth == ImageB::Depth,
                "ImageA and ImageB must have same depth.");

            ImageB srcx;
            ImageB srcy;
            applyX(srcImg, srcx);
            applyY(srcImg, srcy);

            destImg = (srcx * srcx + srcy * srcy).sqrt();
        }
    };
}

#endif
