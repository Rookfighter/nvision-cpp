/* box_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_BOX_FILTER_H_
#define CVE_BOX_FILTER_H_

#include "cve/core/kernel.h"

namespace cve
{
    /** Filter class to apply an iterated box blur.
     *
     *  It applies a 2D average operation on each pixel with kernel:
     *        | 1 1 1 |
     *  1/9 * | 1 1 1 |
     *        | 1 1 1 |
     *
     *  The iterated box filter becomes a Gaussian filter in the limit
     *  (iterations -> inf).
     *  @tparam Scalar value type of the underlying kernel
     *  @tparam Dim size of the underlying kernel */
    template<typename Scalar>
    class BoxFilter
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    private:
        Index iterations_;
        BorderHandling handling_;

        Vector kernel_;
    public:

        BoxFilter()
            : BoxFilter(3, 1)
        {
        }

        BoxFilter(const Index ksize, const Index iterations)
            : iterations_(iterations), handling_(BorderHandling::Reflect),
            kernel_(ksize)
        {
            setKernelSize(ksize);
        }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        void setIterations(const Index iterations)
        {
            iterations_ = iterations;
        }

        void setKernelSize(const Index ksize)
        {
            kernel_.setOnes(ksize);
            kernel_ /= ksize;
        }

        Index kernelSize() const
        {
            return kernel_.size();
        }

        Matrix kernel() const
        {
            return kernel_ * kernel_.transpose();
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Tensor<ScalarB, 3> tmpImg(srcImg.dimensions());
            destImg.resize(srcImg.dimensions());
            destImg = srcImg.template cast<ScalarB>();

            for(Index i = 0; i < iterations_; ++i)
            {
                kernel::apply(destImg, tmpImg, kernel_, handling_);
                kernel::apply(tmpImg, destImg, kernel_.transpose(), handling_);
            }
        }

        template<typename ScalarA>
        void operator()(Eigen::Tensor<ScalarA, 3> &img) const
        {
            Eigen::Tensor<ScalarA, 3> tmp;
            operator()(img, tmp);
            img = tmp;
        }
    };
}

#endif
