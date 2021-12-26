/* homography_decomposition.h
 *
 * Author: Fabian Meyer
 * Created On: 25 Sep 2019
 */

#ifndef CVE_HOMOGRAPHY_DECOMPOSITION_H_
#define CVE_HOMOGRAPHY_DECOMPOSITION_H_

#include <vector>
#include "cve/core/assert.h"
#include "cve/core/matrix.h"

namespace cve
{
    /** Functor to compute the direct linear transformation between two images,
      * given sufficient amount of point correspondences. */
    template<typename Scalar>
    class HomographyDecomposition
    {
    public:
        typedef Eigen::Matrix<Scalar, 6, 1> Vector6;
        typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    private:
        Vector6 computeVector(const Matrix3 &H, const Index i, const Index j) const
        {
            Vector6 result;
            result <<
                H(0, i) * H(0, j),
                H(0, i) * H(1, j) + H(1, i) * H(0, j),
                H(1, i) * H(1, j),
                H(2, i) * H(0, j) + H(0, i) * H(2, j),
                H(2, i) * H(1, j) + H(1, i) * H(2, j),
                H(2, i) * H(2, j);
            return result;
        }
    public:
        HomographyDecomposition()
        { }

        Matrix3 operator()(const std::vector<Matrix3> &homographies) const
        {
            CVE_ASSERT(homographies.size() >= 3);

            Index cnt = homographies.size();

            Matrix A(2 * cnt, 6);
            for(Index i = 0; i < cnt; ++i)
            {
                const Matrix3 &H = homographies[i];

                A.row(2*i) = computeVector(H, 0, 1).transpose();
                A.row(2*i+1) = (computeVector(H, 0, 0) - computeVector(H, 1, 1)).transpose();
            }

            // solve the linear equation system
            // it is homogeneous, so only V matrix is needed
            Eigen::JacobiSVD<Matrix> svd(A, Eigen::ComputeFullV);
            Vector6 vecB = svd.matrixV().col(5);

            Scalar y0 = (vecB(1) * vecB(3) - vecB(0) * vecB(4)) / (vecB(0) * vecB(2) - vecB(1) * vecB(1));
            Scalar lambda = vecB(5) - (vecB(3) * vecB(3) + y0 * (vecB(1) * vecB(3) - vecB(0) * vecB(4))) / vecB(0);
            Scalar alphax = std::sqrt(lambda / vecB(0));
            Scalar alphay = std::sqrt(lambda * vecB(0) / (vecB(0) * vecB(2) - vecB(1) * vecB(1)));
            Scalar s = -vecB(1) * alphax * alphax * alphay / lambda;
            Scalar x0 = s * y0 / alphay - vecB(3) * alphax * alphax / lambda;

            Matrix3 K;
            K << alphax, s, x0,
                0, alphay, y0,
                0, 0, 1;
            return K;
        }
    };

}

#endif
