/* kernel.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_KERNEL_H_
#define CVE_KERNEL_H_

namespace cve
{
    template<typename Scalar, int rows, int cols>
    class Kernel
    {
    public:
        typedef Eigen::Matrix<Scalar, rows, cols> Matrix;
        typedef Matrix::Index Index;

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

        template<typename Image>
        void apply(const Image &img, Image &outImg) const
        {
            outImg.resize(img.rows(), img.cols());

            #pragma omp parallel for collapse(2)
            for(Index col = 0; col < img.cols(); ++col)
            {
                for(Index row = 0; row < img.rows(); ++row)
                {
                    Image::Scalar accum = 0;
                    for(Index kcol = 0; kcol < matrix_.cols(); ++kcol)
                    {
                        Index offsetCol = kcol - matrix_.cols() / 2;
                        Index icol = col + offsetCol;

                        // check bounds and reflect if necessary
                        if(icol < 0)
                            icol = -icol;
                        if(icol >= img.cols())
                            icol = img.cols() - (icol - img.cols());

                        for(Index krow = 0; krow < matrix_.rows(); ++krow)
                        {
                            Index offsetRow = krow - matrix_.rows() / 2;
                            Index irow = row + offsetRow;

                            // check bounds and reflect if necessary
                            if(irow < 0)
                                irow = -irow;
                            if(irow >= img.rows())
                                irow = img.rows() - (irow - img.rows());

                            accum += matrix_(krow, kcol) * img(irow, icol);
                        }
                    }
                    outImg(row, col) = accum;
                }
            }
        }
    };
}

#endif
