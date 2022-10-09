/* direct_linear_transformation.h
 *
 * Author: Fabian Meyer
 * Created On: 10 Sep 2019
 */

#ifndef NVISION_DIRECT_LINEAR_TRANSFORMATION_H_
#define NVISION_DIRECT_LINEAR_TRANSFORMATION_H_

#include "nvision/projective/point_normalization.h"

namespace nvision
{
    /** Functor to compute the direct linear transformation between two images,
      * given sufficient amount of point correspondences. */
    template<typename Scalar>
    class DirectLinearTransformation
    {
    public:
        typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
        typedef Eigen::Matrix<Scalar, 9, 1> Vector9;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;

        DirectLinearTransformation()
        { }

        template<typename DerivedA, typename DerivedB>
        Matrix3 operator()(const Eigen::MatrixBase<DerivedA> &pointsA,
            const Eigen::MatrixBase<DerivedB> &pointsB) const
        {
            NVISION_ASSERT(pointsA.rows() == 2);
            NVISION_ASSERT(pointsB.rows() == 2);
            NVISION_ASSERT(pointsA.cols() >= 3);
            NVISION_ASSERT(pointsA.cols() == pointsB.cols());

            PointNormalization2D<Scalar> normalizer;

            // compute normalization transforms for points A and B independently
            // centroid of each point set will be at (0, 0) and average length
            // at sqrt(2)
            Matrix3 TA = normalizer(pointsA);
            Matrix3 TB = normalizer(pointsB);

            // compute linear equation system for DLT algorithm
            Matrix A(2 * pointsA.cols(), 9);
            for(Index i = 0; i < pointsA.cols(); ++i)
            {
                // transform each point according to normalization transforms
                Vector2 pA = (TA * pointsA.col(i).homogeneous()).hnormalized();
                Vector2 pB = (TB * pointsB.col(i).homogeneous()).hnormalized();

                A.row(2*i) <<
                    0,
                    0,
                    0,
                    -pA(0),
                    -pA(1),
                    -1,
                    pB(1) * pA(0),
                    pB(1) * pA(1),
                    pB(1);

                A.row(2*i+1) <<
                    pA(0),
                    pA(1),
                    1,
                    0,
                    0,
                    0,
                    -pB(0) * pA(0),
                    -pB(0) * pA(1),
                    -pB(0);
            }

            // solve the linear equation system
            // it is homogeneous, so only V matrix is needed
            Eigen::JacobiSVD<Matrix> svd(A, Eigen::ComputeFullV);
            Vector9 vecH = svd.matrixV().col(8);
            // put vectorized result back in matrix form
            Matrix3 H;
            H << vecH(0), vecH(1), vecH(2),
                vecH(3), vecH(4), vecH(5),
                vecH(6), vecH(7), vecH(8);
            // incorporate normalization transforms
            H = TB.inverse() * H * TA;
            // normalize homogeneous transform
            H /= H(2, 2);

            return H;
        }
    };
}

#endif
