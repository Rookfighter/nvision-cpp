/* sobel_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 30 Apr 2019
 */

#ifndef CVE_SOBEL_FILTER_H_
#define CVE_SOBEL_FILTER_H_

#include "cve/kernel.h"

namespace cve
{
    /** Filter class to compute Sobel gradients.
     *  The Sobel operator computes the first derivative of the image.
     *  @tparam Scalar value type of the underlying kernel*/
    template<typename Scalar>
    class SobelFilter
    {
    private:
        BorderHandling handling_;
    public:
        SobelFilter()
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

            Eigen::Matrix<Scalar, 1, 3> kernelX;
            Eigen::Matrix<Scalar, 3, 1> kernelY;

            kernelX << -1, 0, 1;
            kernelY << 1, 2, 1;

            ImageB tmpImg;
            kernel::apply(srcImg, tmpImg, kernelX, handling_);
            kernel::apply(tmpImg, destImg, kernelY, handling_);
        }

        template<typename ImageA, typename ImageB>
        void applyY(const ImageA &srcImg,
            ImageB &destImg) const
        {
            static_assert(ImageA::Depth == ImageB::Depth,
                "ImageA and ImageB must have same depth.");

            Eigen::Matrix<Scalar, 1, 3> kernelX;
            Eigen::Matrix<Scalar, 3, 1> kernelY;

            kernelX << 1, 2, 1;
            kernelY << -1, 0, 1;

            ImageB tmpImg;
            kernel::apply(srcImg, tmpImg, kernelX, handling_);
            kernel::apply(tmpImg, destImg, kernelY, handling_);
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
