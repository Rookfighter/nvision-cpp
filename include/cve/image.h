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
