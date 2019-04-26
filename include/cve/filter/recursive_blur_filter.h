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
    template<typename Scalar>
    class RecursiveBlurFilter
    {
    private:
        Scalar sigma_;

    public:
        RecursiveBlurFilter()
            :sigma_(1)
        {
        }

        RecursiveBlurFilter(const Scalar sigma)
            :sigma_(sigma)
        {
        }

        void setSigma(const Scalar sigma)
        {
            sigma_ = sigma;
        }


        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            // typedef Eigen::Array<Eigen::Array<Scalar, Image::Scalar::Rows, Image::Scalar::Cols>, Eigen::Dynamic, Eigen::Dynamic> ScalarArray;

            outImg.resize(img.rows(), img.cols());
            Image fimg(img.rows(), img.cols());
            Image gimg(img.rows(), img.cols());

            Scalar smoothness = 5.0 / (sigma_ * 2 * 1.772453850905516);
            Scalar smoothExp = std::exp(-smoothness);
            Scalar factor = (1 - smoothExp) * (1 - smoothExp) /
                ((1 + 2 * smoothness * smoothExp) - (smoothExp * smoothExp));

            // process filter horizontally
            for(Index c = 0; c < img.cols(); ++c)
            {
                Index c2 = img.cols() - 1 - c;

                for(Index r = 0; r < img.rows(); ++r)
                {
                    for(Index d = 0; d < img(r, c).size(); ++d)
                    {
                        Scalar fval = factor * img(r, c)(d);
                        Scalar gval = 0;

                        if(c > 0)
                        {
                            fval += factor * smoothExp * (smoothness - 1) * img(r, c - 1)(d) + 2 * smoothExp * fimg(r, c - 1)(d);
                            gval += factor * smoothExp * (smoothness + 1) * img(r, c2 + 1)(d) + 2 * smoothExp * gimg(r, c2 + 1)(d);
                        }
                        if(c > 1)
                        {
                            fval -= smoothExp * smoothExp * fimg(r, c - 2)(d);
                            gval -= smoothExp * smoothExp * gimg(r, c2 + 2)(d) + factor * smoothExp * smoothExp * img(r, c2 + 2)(d);
                        }
                        fimg(r, c)(d) = fval;
                        gimg(r, c2)(d) = gval;
                    }
                }
            }

            outImg = gimg + fimg;
            for(Index r = 0; r < img.rows(); ++r)
            {
                Index r2 = img.rows() - 1 - r;

                for(Index c = 0; c < img.cols(); ++c)
                {
                    for(Index d = 0; d < img(r, c).size(); ++d)
                    {
                        Scalar fval = factor * outImg(r, c)(d);
                        Scalar gval = 0;

                        if(r > 0)
                        {
                            fval += factor * smoothExp * (smoothness - 1) * outImg(r - 1, c)(d) + 2 * smoothExp * fimg(r - 1, c)(d);
                            gval += factor * smoothExp * (smoothness + 1) * outImg(r2 + 1, c)(d) + 2 * smoothExp * gimg(r2 + 1, c)(d);
                        }
                        if(r > 1)
                        {
                            fval -= smoothExp * smoothExp * fimg(r - 2, c)(d);
                            gval -= smoothExp * smoothExp * gimg(r2 + 2, c)(d) + factor * smoothExp * smoothExp * outImg(r2 + 2, c)(d);
                        }
                        fimg(r, c)(d) = fval;
                        gimg(r2, c)(d) = gval;
                    }
                }
            }

            outImg = gimg + fimg;

        }
    };
}

#endif
