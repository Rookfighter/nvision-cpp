/* recursive_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_RECURSIVE_FILTER_H_
#define CVE_RECURSIVE_FILTER_H_

#include <Eigen/Geometry>

namespace cve
{
    template<typename Scalar>
    class RecursiveFilter
    {
    private:
        typedef Eigen::Array<Scalar, Eigen::Dynamic, 1> ArrayVector;
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
        typedef typename ArrayVector::Index Index;

        Scalar sigma_;

        Scalar calculateForward(const Scalar currImg,
            const Scalar prevImg,
            const Scalar prevForward1,
            const Scalar prevForward2,
            const Scalar expalpha,
            const Scalar alpha) const
        {
            return currImg
                + expalpha * (alpha - 1) * prevImg
                + 2 * expalpha * prevForward1
                - expalpha * expalpha * prevForward2;
        }

        template<typename Image>
        void processForwardY(Index row,
            Index col,
            Index d,
            const Scalar expalpha,
            const Scalar alpha,
            const Image &img,
            ArrayVector &fimg) const
        {
            Index width = img.cols();
            Index height = img.rows();
            Index depth = img(row, col).size();

            Index idx = col * height * depth + row * depth + d;

            Scalar currImg = img(row, col)(d);
            Scalar prevImg = 0;
            Scalar prev1 = 0;
            Scalar prev2 = 0;

            if(row > 0)
            {
                prevImg = img(row - 1, col)(d);
                prev1 = fimg(idx - depth);
            }

            if(row > 1)
                prev2 = fimg(idx - 2 * depth);

            fimg(idx) = calculateForward(currImg, prevImg, prev1, prev2,
                expalpha, alpha);
        }

        template<typename Image>
        void processForwardX(Index row,
            Index col,
            Index d,
            const Scalar expalpha,
            const Scalar alpha,
            const Image &img,
            ArrayVector &fimg) const
        {
            Index width = img.cols();
            Index height = img.rows();
            Index depth = img(row, col).size();

            Index idx = col * height * depth + row * depth + d;

            Scalar currImg = img(row, col)(d);
            Scalar prevImg = 0;
            Scalar prev1 = 0;
            Scalar prev2 = 0;

            if(col > 0)
            {
                prevImg = img(row, col - 1)(d);
                prev1 = fimg(idx - height * depth);
            }

            if(col > 1)
                prev2 = fimg(idx - 2 * height * depth);

            fimg(idx) = calculateForward(currImg, prevImg, prev1, prev2,
                expalpha, alpha);
        }

        Scalar calculateBackward(const Scalar prevImg1,
            const Scalar prevImg2,
            const Scalar prevBackward1,
            const Scalar prevBackward2,
            const Scalar expalpha,
            const Scalar alpha) const
        {
            return expalpha * (alpha + 1) * prevImg1
                - expalpha * expalpha * prevImg2
                + 2 * expalpha * prevBackward1
                - expalpha * expalpha * prevBackward2;
        }

        template<typename Image>
        void processBackwardY(Index row,
            Index col,
            Index d,
            const Scalar expalpha,
            const Scalar alpha,
            const Image &img,
            ArrayVector &gimg) const
        {
            Index width = img.cols();
            Index height = img.rows();
            Index depth = img(row, col).size();

            row = height - row - 1;

            Index idx = col * height * depth + row * depth + d;

            Scalar prevImg1 = 0;
            Scalar prevImg2 = 0;
            Scalar prev1 = 0;
            Scalar prev2 = 0;

            if(row < height - 1)
            {
                prevImg1 = img(row + 1, col)(d);
                prev1 = gimg(idx + depth);
            }

            if(row < height - 2)
            {
                prevImg1 = img(row + 2, col)(d);
                prev2 = gimg(idx + 2 * depth);
            }

            gimg(idx) = calculateBackward(prevImg1, prevImg2, prev1, prev2,
                expalpha, alpha);
        }

        template<typename Image>
        void processBackwardX(Index row,
            Index col,
            Index d,
            const Scalar expalpha,
            const Scalar alpha,
            const Image &img,
            ArrayVector &gimg) const
        {
            Index width = img.cols();
            Index height = img.rows();
            Index depth = img(row, col).size();

            col = width - col - 1;

            Index idx = col * height * depth + row * depth + d;

            Scalar prevImg1 = 0;
            Scalar prevImg2 = 0;
            Scalar prev1 = 0;
            Scalar prev2 = 0;

            if(col < width - 1)
            {
                prevImg1 = img(row, col + 1)(d);
                prev1 = gimg(idx + height * depth);
            }

            if(col < width - 2)
            {
                prevImg2 = img(row, col + 2)(d);
                prev2 = gimg(idx + 2 * height * depth);
            }

            gimg(idx) = calculateBackward(prevImg1, prevImg2, prev1, prev2,
                expalpha, alpha);
        }
    public:
        RecursiveFilter()
            :sigma_(1)
        {
        }

        RecursiveFilter(const Scalar sigma)
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
            Index width = img.cols();
            Index height = img.rows();
            Index depth = img(0,0).size();

            const Scalar sqrtpi = 1.772453850905516;
            Scalar alpha = 5.0 / (sigma_ * sqrtpi * 2);
            Scalar expalpha = std::exp(-alpha);
            Scalar factor = 1 - expalpha;
            factor *= factor;
            factor /= 1 + 2 * alpha * expalpha - expalpha * expalpha;

            outImg.resize(height, width);

            ArrayVector fimgX(height * width * depth);
            ArrayVector gimgX(height * width * depth);
            ArrayVector fimgY(height * width * depth);
            ArrayVector gimgY(height * width * depth);

            for(Index col = 0; col < width; ++col)
            {
                for(Index row = 0; row < height; ++row)
                {
                    for(Index d = 0; d < depth; ++d)
                    {
                        processForwardY<Image>(row, col, d, expalpha, alpha, img, fimgY);
                        processBackwardY<Image>(row, col, d, expalpha, alpha, img, gimgY);
                        processForwardX<Image>(row, col, d, expalpha, alpha, img, fimgX);
                        processBackwardX<Image>(row, col, d, expalpha, alpha, img, gimgX);
                    }
                }
            }

            fimgX += gimgX;
            fimgY += gimgY;
            fimgX *= factor;
            fimgY *= factor;

            for(Index col = 0; col < width; ++col)
            {
                for(Index row = 0; row < height; ++row)
                {
                    for(Index d = 0; d < depth; ++d)
                    {
                        Index idx = col * height * depth + row * depth + d;

                        Vector2 vec;
                        vec << fimgX(idx), fimgY(idx);
                        outImg(row, col)(d) = vec.norm();
                    }
                }
            }
        }
    };
}

#endif
