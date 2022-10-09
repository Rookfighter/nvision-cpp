/* point_triangulation.h
 *
 * Author: Fabian Meyer
 * Created On: 10 Sep 2019
 */

#ifndef NVISION_POINT_TRIANGULATION_H_
#define NVISION_POINT_TRIANGULATION_H_

#include "nvision/core/matrix.h"
#include "nvision/core/assert.h"

namespace nvision
{
    template<typename Scalar>
    class PointTriangulation
    {
    public:
        typedef Eigen::Matrix<Scalar, 3, 4> Matrix34;
        typedef Eigen::Matrix<Scalar, 4, 4> Matrix4;
    private:
        Matrix34 PA_;
        Matrix34 PB_;
    public:
        PointTriangulation()
            : PA_(), PB_()
        { }

        PointTriangulation(const Matrix34 &PA, const Matrix34 &PB)
            : PA_(PA), PB_(PB)
        { }

        template<typename DerivedA, typename DerivedB, typename DerivedC>
        void operator()(const Eigen::MatrixBase<DerivedA> &pointsA,
            const Eigen::MatrixBase<DerivedB> &pointsB,
            Eigen::MatrixBase<DerivedC> &outPoints) const
        {
            NVISION_ASSERT(pointsA.rows() == 2);
            NVISION_ASSERT(pointsB.rows() == 2);
            NVISION_ASSERT(outPoints.rows() == 3);
            NVISION_ASSERT(pointsA.cols() == pointsB.cols());
            NVISION_ASSERT(pointsA.cols() == outPoints.cols());

            for(Index i = 0; i < pointsA.cols(); ++i)
            {
                // build a linear equation system for each point
                Matrix4 A;
                for(Index j = 0; j < A.cols(); ++j)
                {
                    A(0, j) = pointsA(0, i) * PA_(2, j) - PA_(0, j);
                    A(1, j) = pointsA(1, i) * PA_(2, j) - PA_(1, j);
                    A(2, j) = pointsB(0, i) * PB_(2, j) - PB_(0, j);
                    A(3, j) = pointsB(1, i) * PB_(2, j) - PB_(1, j);
                }

                // solve by SVD
                // it is homogeneous, so only V matrix is needed
                Eigen::JacobiSVD<Matrix4> svd(A, Eigen::ComputeFullV);
                // store result as non-homogeneous 3D point
                outPoints.col(i) = svd.matrixV().col(3).hnormalized();
            }
        }
    };
}

#endif
