/* box_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_BOX_FILTER_H_
#define CVE_BOX_FILTER_H_

#include "cve/border_handling.h"

namespace cve
{
    /** Implements an iterated box blur filter. It applies a 2D average
     *  operation on each pixel. Its kernel basically looks as follows:
     *        | 1 1 1 |
     *  1/9 * | 1 1 1 |
     *        | 1 1 1 |
     *
     *  The iterated box filter becomes a Gaussian filter in the limit
     *  (iterations -> inf). */
    template<typename Scalar, unsigned int Dim = 3>
    class BoxFilter
    {
    public:
        static_assert(Dim % 2 == 1, "BoxFilter dimension must be odd");
    private:
        size_t iterations_;
        BorderHandling handling_;

        Eigen::Matrix<Scalar, 1, Dim> kernelX_;
        Eigen::Matrix<Scalar, Dim, 1> kernelY_;
    public:

        BoxFilter()
            : BoxFilter(1)
        {
        }

        BoxFilter(const size_t iterations)
            : iterations_(iterations), handling_(BorderHandling::Reflect),
            kernelX_(), kernelY_()
        {
            kernelX_.setOnes();
            kernelX_ /= Dim;

            kernelY_.setOnes();
            kernelY_ /= Dim;
        }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            Image tmpImg;
            outImg = img;

            for(size_t i = 0; i < iterations_; ++i)
            {
                kernel::apply(outImg, tmpImg, kernelX_, handling_);
                kernel::apply(tmpImg, outImg, kernelY_, handling_);
            }
        }
    };
}

#endif
