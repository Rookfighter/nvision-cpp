/* homography_decomposition.cpp
 *
 * Author: Fabian Meyer
 * Created On: 25 Sep 2019
 */

#include "assert/eigen_require.h"
#include <cve/projective/homography_decomposition.h>

using namespace cve;

typedef double Scalar;
typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;

TEST_CASE("homography_decomposition")
{
    Scalar eps = 1e-3;

    std::vector<Matrix3> homographies(5);
    homographies[0] <<
        59.8965, -3.89697, 59.52,
        -1.35894, 61.7844, 439.424,
        -0.0107562, -0.00727671, 1;
    homographies[1] <<
        59.5114, 3.83906, 74.3166,
        -0.334423, 63.5647, 439.516,
        -0.00675643, 0.0136463, 1;
    homographies[2] <<
        44.5274, -4.00555, 134.365,
        -6.12087, 56.0617, 425.074,
        -0.0273251, -0.00645069, 1;
    homographies[3] <<
        68.1272, -3.42147, 80.6779,
        4.56368, 63.6345, 445.151,
        0.0115181, -0.00740877, 1;
    homographies[4] <<
        58.3498, -10.6626, 71.6041,
        13.0522, 56.3232, 390.012,
        0.010377, 0.00186643, 1;

    HomographyDecomposition<Scalar> decomp;

    Matrix3 K = decomp(homographies);

    Matrix3 Kexp;
    Kexp <<
        871.906, -0.383464, 293.669,
        0, 871.727, 212.447,
        0, 0, 1;
    REQUIRE_MATRIX_APPROX(Kexp, K, eps);
}
