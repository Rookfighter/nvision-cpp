/* point_normalization.cpp
 *
 * Author: Fabian Meyer
 * Created On: 26 Sep 2019
 */

#include "assert/eigen_require.h"
#include <cve/projective/point_normalization.h>

using namespace cve;

typedef double Scalar;
typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;

TEST_CASE("point_normalization")
{
    const Scalar eps = 1e-6;
    PointNormalization2D<Scalar> normalizer;

    Matrix points(2, 5);
    points <<
        1, 3, -12, 37, -0.5,
        -1, 14, 7, 401, 21;

    Vector2 centroid = points.rowwise().mean();
    Scalar avgdist = (points.colwise() - centroid).colwise().norm().mean();

    REQUIRE(Approx(0).margin(eps) != centroid(0));
    REQUIRE(Approx(0).margin(eps) != centroid(1));
    REQUIRE(Approx(std::sqrt(2)).margin(eps) != avgdist);

    Matrix3 T = normalizer(points);

    Matrix pointsAct = (T * points.colwise().homogeneous()).colwise().hnormalized();
    Vector2 centroidAct = pointsAct.rowwise().mean();
    Scalar avgdistAct = (pointsAct.colwise() - centroidAct).colwise().norm().mean();

    REQUIRE(Approx(0).margin(eps) == centroidAct(0));
    REQUIRE(Approx(0).margin(eps) == centroidAct(1));
    REQUIRE(Approx(std::sqrt(2)).margin(eps) == avgdistAct);
}
