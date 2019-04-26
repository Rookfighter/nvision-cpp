/* kernel.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_KERNEL_H_
#define CVE_KERNEL_H_

#include "cve/image.h"

namespace cve
{
    template<typename Scalar, unsigned int Dim>
    class Kernel
    {
    public:
        static_assert(Dim % 2 == 1, "Kernel dimension must be odd");

        typedef Eigen::Matrix<Scalar, Dim, Dim> Matrix;

        enum class BorderHandling
        {
            Constant = 0,
            Replicate,
            Reflect
        };
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

        Index dimension() const
        {
            return Dim;
        }

        void apply(const Kernel<Scalar, Dim> &kernel, Kernel<Scalar, Dim> &outKernel) const
        {
            for(Index col = 0; col < kernel.matrix().cols(); ++col)
            {
                for(Index row = 0; row < kernel.matrix().rows(); ++row)
                {
                    Scalar accum = 0;

                    for(Index kcol = 0; kcol < matrix_.cols(); ++kcol)
                    {
                        Index offsetCol = kcol - matrix_.cols() / 2;
                        Index icol = border::reflect(col + offsetCol, 0,
                            kernel.matrix().cols());

                        for(Index krow = 0; krow < matrix_.rows(); ++krow)
                        {
                            Index offsetRow = krow - matrix_.rows() / 2;
                            Index irow = border::reflect(row + offsetRow, 0,
                                kernel.matrix().rows());

                            accum += matrix_(krow, kcol) * kernel.matrix()(irow, icol);
                        }
                    }
                    outKernel.matrix()(row, col) = accum;
                }
            }
        }

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            outImg.resize(img.rows(), img.cols());

            for(Index col = 0; col < img.cols(); ++col)
            {
                for(Index row = 0; row < img.rows(); ++row)
                {
                    typename Image::Scalar accum;
                    accum.setZero();

                    for(Index kcol = 0; kcol < matrix_.cols(); ++kcol)
                    {
                        Index offsetCol = kcol - matrix_.cols() / 2;
                        Index icol = border::reflect(col + offsetCol, 0,
                            img.cols());

                        for(Index krow = 0; krow < matrix_.rows(); ++krow)
                        {
                            Index offsetRow = krow - matrix_.rows() / 2;
                            Index irow = border::reflect(row + offsetRow, 0,
                                img.rows());

                            for(Index depth = 0; depth < accum.size(); ++depth)
                                accum(depth) += matrix_(krow, kcol) * img(irow, icol)(depth);
                        }
                    }
                    outImg(row, col) = accum;
                }
            }
        }
    };
}

#endif
