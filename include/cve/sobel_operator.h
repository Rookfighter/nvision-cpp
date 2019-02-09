/* sobel_operator.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_SOBEL_OPERATOR_H_
#define CVE_SOBEL_OPERATOR_H_

namespace cve
{
    template<typename Scalar>
    class SobelOperator
    {
    private:
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
                    outImg(j, i) = 0.5 * Eigen::abs(outImg(j, i)) +
                        0.5 * Eigen::abs(tmp(j, i));
                }
            }
        }

    };
}

#endif
