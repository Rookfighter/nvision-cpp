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
    public:

        BoxFilter()
            : BoxFilter(1)
        { }

        BoxFilter(const size_t iterations)
            : iterations_(iterations), handling_(BorderHandling::Reflect)
        {
        }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            Index dimHalf = Dim / 2;
            Image tmpImg;
            outImg = img;

            for(size_t i = 0; i < iterations_; ++i)
            {
                // copy image to temporary variable
                tmpImg = outImg;
                // compute average of first row of image
                for(Index c = 0; c < img.cols(); ++c)
                {
                    outImg(0, c).setZero();
                    for(Index r = -dimHalf; r < dimHalf + 1; ++r)
                    {
                        // handle borders accordingly
                        Index idx = border::handle(r, 0, img.rows(), handling_);
                        outImg(0, c) += tmpImg(idx, c) / Dim;
                    }
                }

                // compute vertical blur of image
                for(Index c = 0; c < img.cols(); ++c)
                {
                    for(Index r = 1; r < img.rows(); ++r)
                    {
                        // handle borders accordingly
                        Index r1 = border::handle(r + dimHalf, 0, img.rows(), handling_);
                        Index r2 = border::handle(r - 1 - dimHalf, 0, img.rows(), handling_);

                        outImg(r, c) = outImg(r - 1, c) + tmpImg(r1, c) / Dim - tmpImg(r2, c) / Dim;
                    }
                }

                tmpImg = outImg;
                // compute average of first column of image
                for(Index r = 0; r < img.rows(); ++r)
                {
                    outImg(r, 0).setZero();
                    for(Index c = -dimHalf; c < dimHalf + 1; ++c)
                    {
                        // handle border accordingly
                        Index idx = border::handle(c, 0, img.cols(), handling_);
                        outImg(r, 0) += tmpImg(r, idx) / Dim;
                    }
                }

                // compute horizontal blur of image
                for(Index r = 0; r < img.rows(); ++r)
                {
                    for(Index c = 1; c < img.cols(); ++c)
                    {
                        // handle border accordingly
                        Index c1 = border::handle(c + dimHalf, 0, img.cols(), handling_);
                        Index c2 = border::handle(c - 1 - dimHalf, 0, img.cols(), handling_);

                        outImg(r, c) = outImg(r, c - 1) + tmpImg(r, c1) / Dim - tmpImg(r, c2) / Dim;
                    }
                }
            }
        }
    };
}

#endif
