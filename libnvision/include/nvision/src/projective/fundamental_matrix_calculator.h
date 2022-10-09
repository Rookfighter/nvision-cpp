/* fundamental_matrix_calculator.h
 *
 * Author: Fabian Meyer
 * Created On: 10 Sep 2019
 */

#ifndef NVISION_FUNDAMENTAL_MATRIX_CALCULATOR_H_
#define NVISION_FUNDAMENTAL_MATRIX_CALCULATOR_H_

#include <Eigen/Geometry>

namespace nvision
{
    template<typename Scalar>
    class FundamentalMatrixCalculator
    {
    public:
        typedef Eigen::Matrix<Scalar, 3, 4> Matrix34;
        typedef Eigen::Matrix<Scalar, 4, 3> Matrix43;
        typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;
        typedef Eigen::Matrix<Scalar, 3, 1> Vector3;

        Matrix3 operator()(const Matrix34 &PA, const Matrix34 &PB) const
        {
            Matrix43 pseudoInv;
            pseudoInv = (PA.transpose() * PA).inverse() * PA.transpose();
            Vector3 epipole = (PB * Vector3::Zero().homogeneous()).hnormalized();

            Matrix3 skewMat;
            skewMat << 0, -epipole(2), epipole(1),
                    epipole(2), 0, -epipole(0),
                    -epipole(1), epipole(0), 0;

            return skewMat * PB * pseudoInv;
        }
    };
}

#endif
