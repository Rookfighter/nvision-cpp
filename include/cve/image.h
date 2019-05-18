/* image.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_IMAGE_H_
#define CVE_IMAGE_H_

#include <Eigen/Geometry>
#include <unsupported/Eigen/CXX11/Tensor>

namespace cve
{
    typedef long int Index;

    typedef Eigen::Tensor<uint8_t, 3> Image8;
    typedef Eigen::Tensor<uint16_t, 3> Image16;
    typedef Eigen::Tensor<uint32_t, 3> Image32;
    typedef Eigen::Tensor<float, 3> Imagef;
    typedef Eigen::Tensor<double, 3> Imaged;

    typedef Eigen::Tensor<uint8_t, 1> Pixel8;
    typedef Eigen::Tensor<uint16_t, 1> Pixel16;
    typedef Eigen::Tensor<uint32_t, 1> Pixel32;
    typedef Eigen::Tensor<float, 1> Pixelf;
    typedef Eigen::Tensor<double, 1> Pixeld;

    namespace image
    {
        template<typename Scalar>
        void clamp(Eigen::Tensor<Scalar, 3> &img,
            const Eigen::Tensor<Scalar, 1> &minval,
            const Eigen::Tensor<Scalar, 1> &maxval)
        {
            for(Index d = 0; d < img.dimension(2); ++d)
            {
                for(Index c = 0; c < img.dimension(1); ++c)
                {
                    for(Index r = 0; r < img.dimension(0); ++r)
                    {
                        img(r, c, d) = std::min(maxval(d), std::max(minval(d),
                            img(r, c, d)));
                    }
                }
            }
        }

        template<typename Scalar>
        void normalize(Eigen::Tensor<Scalar, 3> &img,
            const Eigen::Tensor<Scalar, 1> &minval,
            const Eigen::Tensor<Scalar, 1> &maxval)
        {
            Eigen::Tensor<Scalar, 1> oldMin = img.minimum({0, 1});
            Eigen::Tensor<Scalar, 1> oldMax = img.maximum({0, 1});;

            Eigen::Tensor<Scalar, 1> factor = (maxval - minval) / (oldMax - oldMin);
            img = (img - oldMin) * factor + minval;
        }
    }

    template<typename ScalarA, typename ScalarB>
    void downsample(const ScalarA factor,
        const Eigen::Tensor<ScalarB, 3> &srcImg,
        Eigen::Tensor<ScalarB, 3> &destImg)
    {
        destImg.resize(srcImg.dimensions());
        Eigen::Tensor<ScalarB, 3> tmp(srcImg.dimensions());

        // process downsample vertically
        for(Index d = 0; d < srcImg.dimension(2); ++d)
        {
            for(Index c = 0; c < srcImg.dimension(1); ++c)
            {
                for(Index r = 0; r < srcImg.dimension(0); ++r)
                {
                    ScalarA lowPixel = static_cast<ScalarA>(r) / factor;
                    ScalarA highPixel = static_cast<ScalarA>(r+1) / factor;
                    Index r2 = static_cast<Index>(lowPixel) + 1;

                    ScalarA sum = (static_cast<ScalarA>(r2) - lowPixel) * srcImg(r2, c, d);
                    Index r3 = r2 + 1;
                    while(r3 < srcImg.dimension(0) && r3 < highPixel)
                    {
                        sum += srcImg(r3, c, d);
                        ++r3;
                    }

                    if(r3 < srcImg.dimension(0))
                    {
                        sum += (1 - (static_cast<ScalarA>(r3) - highPixel)) * srcImg(r3, c, d);
                        tmp(r, c, d) = factor * sum;
                    }
                    else
                    {
                        tmp(r, c, d) = 1 / ((1 / factor) -
                            (highPixel - static_cast<ScalarA>(r3) + 1)) * sum;
                    }
                }
            }
        }

        // process downsample horizontally
        for(Index d = 0; d < srcImg.dimension(2); ++d)
        {
            for(Index c = 0; c < srcImg.dimension(1); ++c)
            {
                for(Index r = 0; r < srcImg.dimension(0); ++r)
                {
                    ScalarA lowPixel = static_cast<ScalarA>(c) / factor;
                    ScalarA highPixel = static_cast<ScalarA>(c+1) / factor;
                    Index c2 = static_cast<Index>(lowPixel) + 1;

                    ScalarA sum = (static_cast<ScalarA>(c2) - lowPixel) * tmp(r, c2, d);
                    Index c3 = c2 + 1;
                    while(c3 < tmp.cols() && c3 < highPixel)
                    {
                        sum += tmp(r, c3, d);
                        ++c3;
                    }

                    if(c3 < tmp.cols())
                    {
                        sum += (1 - (static_cast<ScalarA>(c3) - highPixel)) * tmp(r, c3, d);
                        destImg(r, c, d) = factor * sum;
                    }
                    else
                    {
                        destImg(r, c, d) = 1 / ((1 / factor) -
                            (highPixel - static_cast<ScalarA>(c3) + 1)) * sum;
                    }
                }
            }
        }
    }

    template<typename Scalar, typename Image>
    void upsample(const Scalar factor, Image &img)
    {
    }
}

#endif
