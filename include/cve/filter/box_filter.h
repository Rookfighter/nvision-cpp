/* box_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_BOX_FILTER_H_
#define CVE_BOX_FILTER_H_

#include "cve/image.h"

namespace cve
{
    template<typename Scalar, unsigned int Dim = 3>
    class BoxFilter
    {
    public:
        static_assert(Dim % 2 == 1, "BoxFilter dimension must be odd");
    private:
        size_t iterations_;
    public:

        BoxFilter()
            : BoxFilter(1)
        { }

        BoxFilter(const size_t iterations)
            : iterations_(iterations)
        {
        }

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            Index dimHalf = Dim / 2;
            Image tmpImg;
            outImg = img;

            for(size_t i = 0; i < iterations_; ++i)
            {
                tmpImg = outImg;

                // compute first row
                for(Index c = 0; c < img.cols(); ++c)
                {
                    outImg(0, c).setZero();
                    for(Index r = -dimHalf; r < dimHalf + 1; ++r)
                    {
                        Index idx = border::reflect(r, 0, img.rows());
                        outImg(0, c) += tmpImg(idx, c) / Dim;
                    }
                }

                // compute vertical
                for(Index c = 0; c < img.cols(); ++c)
                {
                    for(Index r = 1; r < img.rows(); ++r)
                    {
                        Index r1 = border::reflect(r + dimHalf, 0, img.rows());
                        Index r2 = border::reflect(r - 1 - dimHalf, 0, img.rows());

                        outImg(r, c) = outImg(r - 1, c) + tmpImg(r1, c) / Dim - tmpImg(r2, c) / Dim;
                    }
                }

                tmpImg = outImg;
                // compute first col
                for(Index r = 0; r < img.rows(); ++r)
                {
                    outImg(r, 0).setZero();
                    for(Index c = -dimHalf; c < dimHalf + 1; ++c)
                    {
                        Index idx = border::reflect(c, 0, img.cols());
                        outImg(r, 0) += tmpImg(r, idx) / Dim;
                    }
                }

                // compute horizontal
                for(Index r = 0; r < img.rows(); ++r)
                {
                    for(Index c = 1; c < img.cols(); ++c)
                    {
                        Index c1 = border::reflect(c + dimHalf, 0, img.cols());
                        Index c2 = border::reflect(c - 1 - dimHalf, 0, img.cols());

                        outImg(r, c) = outImg(r, c - 1) + tmpImg(r, c1) / Dim - tmpImg(r, c2) / Dim;
                    }
                }
            }
        }
    };
}

#endif
