/* sobel_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 30 Apr 2019
 */

#ifndef CVE_SOBEL_FILTER_H_
#define CVE_SOBEL_FILTER_H_

#include "cve/core/kernel.h"

namespace cve
{
    /** Filter class to compute Sobel gradients.
     *  The Sobel operator computes the first derivative of the image.
     *  @tparam Scalar value type of the underlying kernel*/
    template<typename Scalar>
    class SobelFilter
    {
    private:
        BorderHandling handling_;
    public:
        SobelFilter()
            : handling_(BorderHandling::Reflect)
        {
        }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        template<typename ScalarA, typename ScalarB>
        void applyX(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Matrix<Scalar, 1, 3> kernelX;
            Eigen::Matrix<Scalar, 3, 1> kernelY;

            kernelX << -1, 0, 1;
            kernelY << 1, 2, 1;

            Eigen::Tensor<ScalarB, 3> tmpImg;
            kernel::apply(srcImg, tmpImg, kernelX, handling_);
            kernel::apply(tmpImg, destImg, kernelY, handling_);
        }

        template<typename ScalarA, typename ScalarB>
        void applyY(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Matrix<Scalar, 1, 3> kernelX;
            Eigen::Matrix<Scalar, 3, 1> kernelY;

            kernelX << 1, 2, 1;
            kernelY << -1, 0, 1;

            Eigen::Tensor<ScalarB, 3> tmpImg;
            kernel::apply(srcImg, tmpImg, kernelX, handling_);
            kernel::apply(tmpImg, destImg, kernelY, handling_);
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImgX,
            Eigen::Tensor<ScalarB, 3> &destImgY) const
        {
            applyX(srcImg, destImgX);
            applyY(srcImg, destImgY);
        }
    };
}

#endif
