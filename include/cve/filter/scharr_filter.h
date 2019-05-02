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
    /** Implements a soble filter. */
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
        void applyX(const ImageA &img,
            ImageB &outImg) const
        {
            Eigen::Matrix<Scalar, 3, 3> kernel;

            kernel << 47, 0, -47,
                      162, 0, -162,
                      47, 0, -47;

            kernel::apply(img, outImg, kernel, handling_);
        }

        template<typename ImageA, typename ImageB>
        void applyY(const ImageA &img,
            ImageB &outImg) const
        {
            Eigen::Matrix<Scalar, 3, 3> kernel;

            kernel << 47, 162, 47,
                      0, 0, 0,
                      -47, -162, -47;

            kernel::apply(img, outImg, kernel, handling_);
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
