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
        template<typename ImageA, typename ImageB, typename Derived>
        void apply(const ImageA &img,
            ImageB &outImg,
            const Eigen::MatrixBase<Derived> &kernel,
            const BorderHandling handling)
        {
            outImg.setZero(img.rows(), img.cols());

            for(Index c = 0; c < img.cols(); ++c)
            {
                for(Index r = 0; r < img.rows(); ++r)
                {
                    for(Index kcol = 0; kcol < kernel.cols(); ++kcol)
                    {
                        Index offsetCol = kcol - kernel.cols() / 2;
                        Index icol = border::handle(c + offsetCol, 0,
                            img.cols(), handling);

                        for(Index krow = 0; krow < kernel.rows(); ++krow)
                        {
                            Index offsetRow = krow - kernel.rows() / 2;
                            Index irow = border::handle(r + offsetRow, 0,
                                img.rows(), handling);

                            for(Index d = 0; d < outImg(r, c).size(); ++d)
                                outImg(r, c)(d) += kernel(krow, kcol) * img(irow, icol)(d);
                        }
                    }
                }
            }
        }
    }

    template<typename Scalar, unsigned int Rows, unsigned int Cols>
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

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            kernel::apply(img, outImg, matrix_, handling_);
        }
    };
}

#endif
