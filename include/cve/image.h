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

    template<typename _Scalar,
        int _Depth>
    class Image : public Eigen::Array<Eigen::Array<_Scalar, _Depth, 1>, Eigen::Dynamic, Eigen::Dynamic>
    {
    public:
        static_assert(_Depth > 0, "Image depth has to be greater than zero");

        typedef Eigen::Array<_Scalar, _Depth, 1> Pixel;
        typedef _Scalar Scalar;
        typedef Eigen::Array<Pixel, Eigen::Dynamic, Eigen::Dynamic> ParentType;
        typedef typename ParentType::Index Index;

        static const int Depth = _Depth;


        Image()
            : ParentType()
        { }

        Image(const Index rows, const Index cols)
            : ParentType(rows, cols)
        { }

        // This constructor allows you to construct Image from Eigen expressions
        template<typename OtherDerived>
        Image(const Eigen::ArrayBase<OtherDerived>& other)
            : ParentType(other)
        { }

        Index depth() const
        {
            return _Depth;
        }

        // This method allows you to assign Eigen expressions to Image
        template<typename OtherDerived>
        Image<Scalar, _Depth>& operator=(const Eigen::ArrayBase <OtherDerived>& other)
        {
            this->ParentType::operator=(other);
            return *this;
        }
    };

    typedef Image<uint8_t, 1> ImageGray;
    typedef Image<uint8_t, 3> ImageRGB;
    typedef Image<uint8_t, 4> ImageRGBA;

    typedef Image<float, 1> ImageGrayf;
    typedef Image<float, 3> ImageRGBf;
    typedef Image<float, 4> ImageRGBAf;

    namespace image
    {
        template<typename Image> void clamp(Image &img,
            const typename Image::Scalar &minval,
            const typename Image::Scalar &maxval)
        {
            for(Index c = 0; c < img.cols(); ++c)
            {
                for(Index r = 0; r < img.rows(); ++r)
                {
                    for(Index d = 0; d < img(r, c).size(); ++d)
                    {
                        img(r, c)(d) = std::min(maxval(d), std::max(minval(d),
                            img(r, c)(d)));
                    }
                }
            }
        }

        template<typename ImageA, typename ImageB> void copy(
            const ImageA &img,
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
                            typename ImageB::Scalar>(
                            img(r, c)(d));
                    }
                }
            }
        }

        template<typename Image> void normalize(Image &img,
            const typename Image::Pixel &minval,
            const typename Image::Pixel &maxval)
        {
            typename Image::Pixel oldMin = img(0, 0);
            typename Image::Pixel oldMax = img(0, 0);;

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

            typename Image::Pixel factor = (maxval - minval) / (oldMax - oldMin);
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
