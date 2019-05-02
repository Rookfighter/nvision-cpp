/* image.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_IMAGE_H_
#define CVE_IMAGE_H_

#include <Eigen/Geometry>

namespace cve
{
    typedef long int Index;

    template<typename Scalar,
        int Depth,
        int Height = Eigen::Dynamic,
        int Width = Eigen::Dynamic>
    struct _Image
    {
        static_assert(Depth > 0, "Image depth has to be greater than zero");

        typedef Eigen::Array<Eigen::Array<Scalar, Depth, 1>, Height, Width> Array;
    };

    typedef _Image<uint8_t, 1>::Array ImageGray;
    typedef _Image<uint8_t, 3>::Array ImageRGB;
    typedef _Image<uint8_t, 4>::Array ImageRGBA;

    typedef _Image<float, 1>::Array ImageGrayf;
    typedef _Image<float, 3>::Array ImageRGBf;
    typedef _Image<float, 4>::Array ImageRGBAf;

    namespace image
    {
        template<typename Image> void clamp(Image &img,
            typename Image::Scalar::Scalar minval,
            typename Image::Scalar::Scalar maxval)
        {
            for(Index c = 0; c < img.cols(); ++c)
            {
                for(Index r = 0; r < img.rows(); ++r)
                {
                    for(Index d = 0; d < img(r, c).size(); ++d)
                    {
                        img(r, c)(d) = std::min(maxval, std::max(minval,
                            img(r, c)(d)));
                    }
                }
            }
        }

        template<typename ImageA, typename ImageB> void copy(const ImageA &img,
            ImageB &outImg)
        {
            outImg.resize(img.rows(), img.cols());

            for(Index c = 0; c < img.cols(); ++c)
            {
                for(Index r = 0; r < img.rows(); ++r)
                {
                    for(Index d = 0; d < img(r, c).size(); ++d)
                    {
                        outImg(r, c)(d) = static_cast<
                            typename ImageB::Scalar::Scalar>(
                            img(r, c)(d));
                    }
                }
            }
        }

        template<typename Image> void normalize(Image &img,
            typename Image::Scalar minval,
            typename Image::Scalar maxval)
        {
            typename Image::Scalar oldMin = img(0, 0);
            typename Image::Scalar oldMax = img(0, 0);;

            for(Index c = 0; c < img.cols(); ++c)
            {
                for(Index r = 0; r < img.rows(); ++r)
                {
                    for(Index d = 0; d < img(r, c).size(); ++d)
                    {
                        if(img(r, c)(d) < oldMin(d))
                            oldMin(d) = img(r, c)(d);
                        if(img(r, c)(d) > oldMax(d))
                            oldMax(d) = img(r, c)(d);
                    }
                }
            }

            typename Image::Scalar factor = (maxval - minval) / (oldMax - oldMin);
            img = (img - oldMin) * factor + minval;
        }
    }

    template<typename Scalar, typename Image>
    void downsample(const Scalar factor, const Image &img, Image &outImg)
    {
        Index width = factor * img.cols();
        Index height = factor * img.rows();

        outImg.resize(height, width);
        Image tmp(height, img.cols());

        // process downsample vertically
        for(Index c = 0; c < tmp.cols(); ++c)
        {
            for(Index r = 0; r < tmp.rows(); ++r)
            {
                Scalar lowPixel = static_cast<Scalar>(r) / factor;
                Scalar highPixel = static_cast<Scalar>(r+1) / factor;
                Index r2 = static_cast<Index>(lowPixel) + 1;

                for(Index d = 0; d < tmp(r, c).size(); ++d)
                {
                    Scalar sum = (static_cast<Scalar>(r2) - lowPixel) * img(r2, c)(d);
                    Index r3 = r2 + 1;
                    while(r3 < img.rows() && r3 < highPixel)
                    {
                        sum += img(r3, c)(d);
                        ++r3;
                    }

                    if(r3 < img.rows())
                    {
                        sum += (1 - (static_cast<Scalar>(r3) - highPixel)) * img(r3, c)(d);
                        tmp(r, c)(d) = factor * sum;
                    }
                    else
                    {
                        tmp(r, c)(d) = 1 / ((1 / factor) -
                            (highPixel - static_cast<Scalar>(r3) + 1)) * sum;
                    }
                }
            }
        }

        // process downsample horizontally
        for(Index r = 0; r < outImg.rows(); ++r)
        {
            for(Index c = 0; c < outImg.cols(); ++c)
            {
                Scalar lowPixel = static_cast<Scalar>(c) / factor;
                Scalar highPixel = static_cast<Scalar>(c+1) / factor;
                Index c2 = static_cast<Index>(lowPixel) + 1;

                for(Index d = 0; d < outImg(r, c).size(); ++d)
                {
                    Scalar sum = (static_cast<Scalar>(c2) - lowPixel) * tmp(r, c2)(d);
                    Index c3 = c2 + 1;
                    while(c3 < tmp.cols() && c3 < highPixel)
                    {
                        sum += tmp(r, c3)(d);
                        ++c3;
                    }

                    if(c3 < tmp.cols())
                    {
                        sum += (1 - (static_cast<Scalar>(c3) - highPixel)) * tmp(r, c3)(d);
                        outImg(r, c)(d) = factor * sum;
                    }
                    else
                    {
                        outImg(r, c)(d) = 1 / ((1 / factor) -
                            (highPixel - static_cast<Scalar>(c3) + 1)) * sum;
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
