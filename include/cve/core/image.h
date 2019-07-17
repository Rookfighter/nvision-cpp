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
            const Scalar &minval,
            const Scalar &maxval)
        {
            Eigen::Tensor<Scalar, 1> oldMin = img.minimum(Eigen::array<int, 2>({0, 1}));
            Eigen::Tensor<Scalar, 1> oldMax = img.maximum(Eigen::array<int, 2>({0, 1}));

            assert(oldMin.dimension(0) == img.dimension(2));
            assert(oldMax.dimension(0) == img.dimension(2));

            for(Index d = 0; d < img.dimension(2); ++d)
            {
                for(Index c = 0; c < img.dimension(1); ++c)
                {
                    for(Index r = 0; r < img.dimension(0); ++r)
                    {
                        Scalar factor = (maxval - minval) / (oldMax(d) - oldMin(d));
                        img(r, c, d) = (img(r, c, d) - oldMin(d)) * factor + minval;
                    }
                }
            }
        }

        template<typename Scalar>
        void gray2rgb(const Eigen::Tensor<Scalar, 3> &src,
            Eigen::Tensor<Scalar, 3> &dest)
        {
            dest.resize(src.dimension(0), src.dimension(1), 3);

            for(Index c = 0; c < src.dimension(1); ++c)
            {
                for(Index r = 0; r < src.dimension(0); ++r)
                {
                    dest(r, c, 0) = src(r, c, 0);
                    dest(r, c, 1) = src(r, c, 0);
                    dest(r, c, 2) = src(r, c, 0);
                }
            }
        }

        template<typename Scalar>
        void rgb2gray(const Eigen::Tensor<Scalar, 3> &src,
            Eigen::Tensor<Scalar, 3> &dest)
        {
            assert(src.dimension(2) >= 3);
            dest.resize(src.dimension(0), src.dimension(1), 1);

            for(Index c = 0; c < src.dimension(1); ++c)
            {
                for(Index r = 0; r < src.dimension(0); ++r)
                {
                    dest(r, c, 0) = 0.2126 * src(r, c, 0) +
                        0.7152 * src(r, c, 1) + 0.0722 * src(r, c, 2);
                }
            }
        }

        template<typename Scalar>
        bool isInside(const Index row, const Index col,
            const Eigen::Tensor<Scalar, 3> &img)
        {
            return row >= 0 && row < img.dimension(0) &&
                col >= 0 && col < img.dimension(1);
        }

        template<typename ScalarA, typename ScalarB>
        void downsample(const ScalarA factor,
            const Eigen::Tensor<ScalarB, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg)
        {
            Index height = factor * srcImg.dimension(0);
            Index width = factor * srcImg.dimension(1);
            Index depth = srcImg.dimension(2);

            Eigen::Tensor<ScalarB, 3> tmpImg(height, srcImg.dimension(1), depth);
            destImg.resize(height, width, depth);

            // process downsample vertically
            for(Index d = 0; d < tmpImg.dimension(2); ++d)
            {
                for(Index c = 0; c < tmpImg.dimension(1); ++c)
                {
                    for(Index r = 0; r < tmpImg.dimension(0); ++r)
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
                            tmpImg(r, c, d) = factor * sum;
                        }
                        else
                        {
                            tmpImg(r, c, d) = 1 / ((1 / factor) -
                                (highPixel - static_cast<ScalarA>(r3) + 1)) * sum;
                        }
                    }
                }
            }

            // process downsample horizontally
            for(Index d = 0; d < destImg.dimension(2); ++d)
            {
                for(Index c = 0; c < destImg.dimension(1); ++c)
                {
                    ScalarA lowPixel = static_cast<ScalarA>(c) / factor;
                    ScalarA highPixel = static_cast<ScalarA>(c+1) / factor;
                    Index c2 = static_cast<Index>(lowPixel) + 1;

                    for(Index r = 0; r < destImg.dimension(0); ++r)
                    {
                        ScalarA sum = (static_cast<ScalarA>(c2) - lowPixel) * tmpImg(r, c2, d);
                        Index c3 = c2 + 1;
                        while(c3 < tmpImg.dimension(1) && c3 < highPixel)
                        {
                            sum += tmpImg(r, c3, d);
                            ++c3;
                        }

                        if(c3 < tmpImg.dimension(1))
                        {
                            sum += (1 - (static_cast<ScalarA>(c3) - highPixel)) * tmpImg(r, c3, d);
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

        template<typename ScalarA, typename ScalarB>
        ScalarA bilinear(const Eigen::Tensor<ScalarA, 3> &img,
            const ScalarB r,
            const ScalarB c,
            const Index d)
        {
            ScalarB r1 = std::floor(r);
            ScalarB r2 = std::ceil(r);
            ScalarB c1 = std::floor(c);
            ScalarB c2 = std::ceil(c);

            ScalarB val11 = img(static_cast<Index>(r1), static_cast<Index>(c1), d);
            ScalarB val12 = img(static_cast<Index>(r1), static_cast<Index>(c2), d);
            ScalarB val21 = img(static_cast<Index>(r2), static_cast<Index>(c1), d);
            ScalarB val22 = img(static_cast<Index>(r2), static_cast<Index>(c2), d);

            // calculate factors in x direction
            ScalarB fac1 = 1;
            ScalarB fac2 = 0;
            if(c2 != c1)
            {
                fac1 = (c2 - c) / (c2 - c1);
                fac2 = (c - c1) / (c2 - c1);
            }

            ScalarB f1 = fac1 * val11 + fac2 * val12;
            ScalarB f2 = fac1 * val21 + fac2 * val22;

            // calculate factors in y direction
            fac1 = 1;
            fac2 = 0;
            if(r2 != r1)
            {
                fac1 = (r2 - r) / (r2 - r1);
                fac2 = (r - r1) / (r2 - r1);
            }

            return static_cast<ScalarA>(fac1 * f1 + fac2 * f2);
        }

        template<typename ScalarA, typename ScalarB>
        void magnitude(const Eigen::Tensor<ScalarA, 3> &x,
            const Eigen::Tensor<ScalarA, 3> &y,
            Eigen::Tensor<ScalarB, 3> &magnitude)
        {
            magnitude = (x * x + y * y).sqrt();
        }

        template<typename ScalarA, typename ScalarB>
        void direction(const Eigen::Tensor<ScalarA, 3> &x,
            const Eigen::Tensor<ScalarA, 3> &y,
            Eigen::Tensor<ScalarB, 3> &direction)
        {
            direction.resize(x.dimensions());
            for(Index d = 0; d < x.dimension(2); ++d)
                for(Index c = 0; c < x.dimension(1); ++c)
                    for(Index r = 0; r < x.dimension(0); ++r)
                        direction(r, c, d) = std::atan2(y(r, c, d), x(r, c, d));
        }
    }
}

#endif
