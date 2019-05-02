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
    /** Implements a soble filter. */
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
        void applyX(const ImageA &img,
            ImageB &outImg) const
        {
            Eigen::Matrix<Scalar, 1, 3> kernelX;
            Eigen::Matrix<Scalar, 3, 1> kernelY;

            kernelX << -1, 0, 1;
            kernelY << 1, 2, 1;

            ImageB tmpImg;
            kernel::apply(img, tmpImg, kernelX, handling_);
            kernel::apply(tmpImg, outImg, kernelY, handling_);
        }

        template<typename ImageA, typename ImageB>
        void applyY(const ImageA &img,
            ImageB &outImg) const
        {
            Eigen::Matrix<Scalar, 1, 3> kernelX;
            Eigen::Matrix<Scalar, 3, 1> kernelY;

            kernelX << 1, 2, 1;
            kernelY << -1, 0, 1;

            ImageB tmpImg;
            kernel::apply(img, tmpImg, kernelX, handling_);
            kernel::apply(tmpImg, outImg, kernelY, handling_);
        }

        template<typename ImageA, typename ImageB>
        void apply(const ImageA &img,
            ImageB &outImg) const
        {
            ImageB imgx;
            ImageB imgy;
            applyX(img, imgx);
            applyY(img, imgy);

            outImg = (imgx * imgx + imgy * imgy).sqrt();
        }
    };
}

#endif
