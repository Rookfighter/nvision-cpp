/* sobel_operator.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_SOBEL_OPERATOR_H_
#define CVE_SOBEL_OPERATOR_H_

#include "cve/kernel.h"

namespace cve
{
    template<typename Scalar>
    class SobelOperator
    {
    private:
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
        Kernel<Scalar, 3, 3> kernelX_;
        Kernel<Scalar, 3, 3> kernelY_;
    public:
        SobelOperator()
        {
            kernelX_.matrix() << -1, 0, 1,
                -2, 0, 2,
                -1, 0, 1;

            kernelY_.matrix() << -1, -2, -1,
                0, 0, 0,
                1, 2, 1;
        }

        template<typename Image>
        void applyX(const Image &img, Image &outImg) const
        {
            kernelX_.apply<Image>(img, outImg);
        }

        template<typename Image>
        void applyY(const Image &img, Image &outImg) const
        {
            kernelY_.apply<Image>(img, outImg);
        }

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            Image tmp;
            applyX<Image>(img, outImg);
            applyY<Image>(img, tmp);

            for(Index i = 0; i < outImg.cols(); ++i)
            {
                for(Index j = 0; j < outImg.rows(); ++j)
                {
                    Vector2 vec(outImg(j, i), tmp(j, i));
                    outImg(j, i) = vec.norm();
                }
            }
        }

    };
}

#endif
