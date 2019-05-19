/* kernel.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_KERNEL_H_
#define CVE_KERNEL_H_

#include "cve/border_handling.h"

namespace cve
{
    namespace kernel
    {
        template<typename ScalarA, typename ScalarB, typename DerivedC>
        void apply(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg,
            const Eigen::MatrixBase<DerivedC> &kernel,
            const BorderHandling handling)
        {
            destImg.resize(srcImg.dimensions());
            destImg.setZero();

            for(Index d = 0; d < srcImg.dimension(2); ++d)
            {
                for(Index c = 0; c < srcImg.dimension(1); ++c)
                {
                    for(Index r = 0; r < srcImg.dimension(0); ++r)
                    {
                        for(Index kcol = 0; kcol < kernel.cols(); ++kcol)
                        {
                            Index offsetCol = kcol - kernel.cols() / 2;
                            Index icol = border::handle(c + offsetCol, 0,
                                srcImg.dimension(1), handling);

                            for(Index krow = 0; krow < kernel.rows(); ++krow)
                            {
                                Index offsetRow = krow - kernel.rows() / 2;
                                Index irow = border::handle(r + offsetRow, 0,
                                    srcImg.dimension(0), handling);

                                destImg(r, c, d) += kernel(krow, kcol) *
                                    srcImg(irow, icol, d);
                            }
                        }
                    }
                }
            }
        }
    }

    template<typename Scalar, int Rows, int Cols>
    class Kernel
    {
    public:
        typedef Eigen::Matrix<Scalar, Rows, Cols> Matrix;
    private:
        Matrix matrix_;
        BorderHandling handling_;

    public:

        Kernel()
            : matrix_(), handling_(BorderHandling::Reflect)
        { }

        Kernel(const Matrix &matrix)
            : matrix_(matrix), handling_(BorderHandling::Reflect)
        { }

        Kernel(const Matrix &matrix, const BorderHandling handling)
            : matrix_(matrix), handling_(handling)
        { }

        void setBorderHandling(const BorderHandling handling)
        {
            handling_ = handling;
        }

        Matrix &matrix()
        {
            return matrix_;
        }

        const Matrix &matrix() const
        {
            return matrix_;
        }

        Index rows() const
        {
            return Rows;
        }

        Index cols() const
        {
            return Cols;
        }

        template<typename ScalarA, typename ScalarB>
        void apply(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            kernel::apply(srcImg, destImg, matrix_, handling_);
        }
    };
}

#endif
