/* eight_point_algorithm.h
 *
 * Author: Fabian Meyer
 * Created On: 26 Sep 2019
 */

#ifndef NVISION_EIGHT_POINT_ALGORITHM_H_
#define NVISION_EIGHT_POINT_ALGORITHM_H_

#include "nvision/projective/point_normalization.h"

namespace nvision
{
    template<typename Scalar>
    class EightPointAlgorithm
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;
        typedef Eigen::DiagonalMatrix<Scalar, 3, 3> DiagonalMatrix3;
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
        typedef Eigen::Matrix<Scalar, 3, 1> Vector3;
        typedef Eigen::Matrix<Scalar, 9, 1> Vector9;

        EightPointAlgorithm()
        { }

        template<typename DerivedA, typename DerivedB>
        Matrix3 operator()(const Eigen::MatrixBase<DerivedA> &pointsA,
            const Eigen::MatrixBase<DerivedB> &pointsB) const
        {
            NVISION_ASSERT(pointsA.rows() == 2);
            NVISION_ASSERT(pointsB.rows() == 2);
            NVISION_ASSERT(pointsA.cols() == pointsB.cols());
            NVISION_ASSERT(pointsA.cols() >= 8);

            PointNormalization2D<Scalar> normalizer;

            // compute normalization transforms for points A and B independently
            // centroid of each point set will be at (0, 0) and average length
            // at sqrt(2)
            Matrix3 TA = normalizer(pointsA);
            Matrix3 TB = normalizer(pointsB);

            // compute linear equation system to be solved
            Matrix A(pointsA.cols(), 9);
            for(Index i = 0; i < pointsA.cols(); ++i)
            {
                // transform each point according to normalization transforms
                Vector2 pA = (TA * pointsA.col(i).homogeneous()).hnormalized();
                Vector2 pB = (TB * pointsB.col(i).homogeneous()).hnormalized();

                A.row(i) <<
                    pB(0) * pA(0),
                    pB(0) * pA(1),
                    pB(0),
                    pB(1) * pA(0),
                    pB(1) * pA(1),
                    pB(1),
                    pA(0),
                    pA(1),
                    1;
            }

            // solve the linear equation system
            // it is homogeneous, so only V matrix is needed
            Eigen::JacobiSVD<Matrix> svd(A, Eigen::ComputeFullV);
            Vector9 vecF = svd.matrixV().col(8);
            // write vector into matrix form
            Matrix3 F;
            F << vecF(0), vecF(1), vecF(2),
                vecF(3), vecF(4), vecF(5),
                vecF(6), vecF(7), vecF(8);
            // incorporate normalization transforms
            F = TB.inverse() * F * TA;

            // enforce rank 2 constraint on fundamental matrix by SVD
            Eigen::JacobiSVD<Matrix3> svdF(F, Eigen::ComputeFullV | Eigen::ComputeFullU);
            Vector3 singvals = svdF.singularValues();
            // set smallest singular value to 0
            singvals(2) = 0;

            F = svdF.matrixU() * DiagonalMatrix3(singvals) * svdF.matrixV().transpose();
            F /= F(2,2);

            return F;
        }

    };
}

#endif
