/* recursive_blur_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_RECURSIVE_BLUR_FILTER_H_
#define CVE_RECURSIVE_BLUR_FILTER_H_

#include "cve/border_handling.h"

namespace cve
{
    /** Filter class to apply a recursive blur.
     *  @tparam Scalar value type for internal computation */
    template<typename Scalar>
    class RecursiveBlurFilter
    {
    private:
        Scalar sigma_;

    public:
        RecursiveBlurFilter()
            : RecursiveBlurFilter(1)
        {
        }

        RecursiveBlurFilter(const Scalar sigma)
            : sigma_(sigma)
        {
        }

        void setSigma(const Scalar sigma)
        {
            sigma_ = sigma;
        }


        template<typename ImageA, typename ImageB>
        void apply(const ImageA &srcImg, ImageB &destImg) const
        {
            destImg.resize(srcImg.rows(), srcImg.cols());
            ImageB fimg(srcImg.rows(), srcImg.cols());
            ImageB gimg(srcImg.rows(), srcImg.cols());

            Scalar smoothness = 5.0 / (sigma_ * 2 * 1.772453850905516);
            Scalar smoothExp = std::exp(-smoothness);
            Scalar factor = (1 - smoothExp) * (1 - smoothExp) /
                ((1 + 2 * smoothness * smoothExp) - (smoothExp * smoothExp));

            // process filter horizontally
            for(Index c = 0; c < srcImg.cols(); ++c)
            {
                Index c2 = srcImg.cols() - 1 - c;

                for(Index r = 0; r < srcImg.rows(); ++r)
                {
                    for(Index d = 0; d < srcImg(r, c).size(); ++d)
                    {
                        Scalar fval = factor * srcImg(r, c)(d);
                        Scalar gval = 0;

                        if(c > 0)
                        {
                            fval += factor * smoothExp * (smoothness - 1) *
                                srcImg(r, c - 1)(d) + 2 * smoothExp *
                                fimg(r, c - 1)(d);
                            gval += factor * smoothExp * (smoothness + 1) *
                                srcImg(r, c2 + 1)(d) + 2 * smoothExp *
                                gimg(r, c2 + 1)(d);
                        }
                        if(c > 1)
                        {
                            fval -= smoothExp * smoothExp * fimg(r, c - 2)(d);
                            gval -= smoothExp * smoothExp *
                                gimg(r, c2 + 2)(d) + factor * smoothExp *
                                smoothExp * srcImg(r, c2 + 2)(d);
                        }
                        fimg(r, c)(d) = fval;
                        gimg(r, c2)(d) = gval;
                    }
                }
            }

            destImg = gimg + fimg;
            for(Index r = 0; r < srcImg.rows(); ++r)
            {
                Index r2 = srcImg.rows() - 1 - r;

                for(Index c = 0; c < srcImg.cols(); ++c)
                {
                    for(Index d = 0; d < srcImg(r, c).size(); ++d)
                    {
                        Scalar fval = factor * destImg(r, c)(d);
                        Scalar gval = 0;

                        if(r > 0)
                        {
                            fval += factor * smoothExp * (smoothness - 1) *
                                destImg(r - 1, c)(d) + 2 * smoothExp *
                                fimg(r - 1, c)(d);
                            gval += factor * smoothExp * (smoothness + 1) *
                                destImg(r2 + 1, c)(d) + 2 * smoothExp *
                                gimg(r2 + 1, c)(d);
                        }
                        if(r > 1)
                        {
                            fval -= smoothExp * smoothExp * fimg(r - 2, c)(d);
                            gval -= smoothExp * smoothExp * gimg(r2 + 2, c)(d) + factor * smoothExp * smoothExp * destImg(r2 + 2, c)(d);
                        }
                        fimg(r, c)(d) = fval;
                        gimg(r2, c)(d) = gval;
                    }
                }
            }

            destImg = gimg + fimg;

        }
    };
}

#endif
