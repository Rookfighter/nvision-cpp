/* recursive_blur_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_RECURSIVE_BLUR_FILTER_H_
#define CVE_RECURSIVE_BLUR_FILTER_H_

#include "cve/core/border_handling.h"

namespace cve
{
    /** Filter functor, which applies a recursive blur. */
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


        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            destImg.resize(srcImg.dimensions());
            Eigen::Tensor<ScalarB, 3> fimg(srcImg.dimensions());
            Eigen::Tensor<ScalarB, 3> gimg(srcImg.dimensions());

            Scalar smoothness = 5.0 / (sigma_ * 2 * 1.772453850905516);
            Scalar smoothExp = std::exp(-smoothness);
            Scalar factor = (1 - smoothExp) * (1 - smoothExp) /
                ((1 + 2 * smoothness * smoothExp) - (smoothExp * smoothExp));

            // process filter horizontally
            for(Index d = 0; d < srcImg.dimension(2); ++d)
            {
                for(Index c = 0; c < srcImg.dimension(1); ++c)
                {
                    Index c2 = srcImg.dimension(1) - 1 - c;

                    for(Index r = 0; r < srcImg.dimension(0); ++r)
                    {
                        Scalar fval = factor * srcImg(r, c, d);
                        Scalar gval = 0;

                        if(c > 0)
                        {
                            fval += factor * smoothExp * (smoothness - 1) *
                                srcImg(r, c - 1, d) + 2 * smoothExp *
                                fimg(r, c - 1, d);
                            gval += factor * smoothExp * (smoothness + 1) *
                                srcImg(r, c2 + 1, d) + 2 * smoothExp *
                                gimg(r, c2 + 1, d);
                        }
                        if(c > 1)
                        {
                            fval -= smoothExp * smoothExp * fimg(r, c - 2, d);
                            gval -= smoothExp * smoothExp *
                                gimg(r, c2 + 2, d) + factor * smoothExp *
                                smoothExp * srcImg(r, c2 + 2, d);
                        }
                        fimg(r, c, d) = fval;
                        gimg(r, c2, d) = gval;
                    }
                }
            }

            destImg = gimg + fimg;
            for(Index d = 0; d < srcImg.dimension(2); ++d)
            {
                for(Index c = 0; c < srcImg.dimension(1); ++c)
                {
                    for(Index r = 0; r < srcImg.dimension(0); ++r)
                    {
                        Index r2 = srcImg.dimension(0) - 1 - r;
                        Scalar fval = factor * destImg(r, c, d);
                        Scalar gval = 0;

                        if(r > 0)
                        {
                            fval += factor * smoothExp * (smoothness - 1) *
                                destImg(r - 1, c, d) + 2 * smoothExp *
                                fimg(r - 1, c, d);
                            gval += factor * smoothExp * (smoothness + 1) *
                                destImg(r2 + 1, c, d) + 2 * smoothExp *
                                gimg(r2 + 1, c, d);
                        }
                        if(r > 1)
                        {
                            fval -= smoothExp * smoothExp * fimg(r - 2, c, d);
                            gval -= smoothExp * smoothExp * gimg(r2 + 2, c, d) +
                                factor * smoothExp * smoothExp *
                                destImg(r2 + 2, c, d);
                        }
                        fimg(r, c, d) = fval;
                        gimg(r2, c, d) = gval;
                    }
                }
            }
            destImg = gimg + fimg;
        }

        template<typename ScalarA>
        void operator()(Eigen::Tensor<ScalarA, 3> &img) const
        {
            Eigen::Tensor<ScalarA, 3> tmp;
            operator()(img, tmp);
            img = tmp;
        }
    };
}

#endif
