/* central_differences_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef CVE_CENTRAL_DIFFERENCES_FILTER_H_
#define CVE_CENTRAL_DIFFERENCES_FILTER_H_

#include "cve/core/kernel.h"

namespace cve
{
    /** Filter class to compute central differences gradients.
     *  @tparam Scalar value type of the underlying kernel*/
    template<typename Scalar>
    class CentralDifferencesFilter
    {
    private:
        BorderHandling handling_;
    public:
        CentralDifferencesFilter()
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
            Eigen::Matrix<Scalar, 1, 3> kernel;
            kernel << -1, 0, 1;

            kernel::apply(srcImg, destImg, kernel, handling_);
        }

        template<typename ScalarA, typename ScalarB>
        void applyY(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Matrix<Scalar, 3, 1> kernel;
            kernel << -1, 0, 1;

            kernel::apply(srcImg, destImg, kernel, handling_);
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
