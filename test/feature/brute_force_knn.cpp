/* brute_force_matcher.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jul 2019
 */

#include "assert/eigen_require.h"
#include <cve/feature/brute_force_knn.h>

using namespace cve;

typedef float Scalar;
typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;

TEST_CASE("brute_force_knn")
{
    SECTION("query one")
    {
        BruteForceKNN<Scalar, EuclideanDistance<Scalar>> matcher;
        Matrix data(3, 4);
        data << 1, 3, 0, 5,
            0, 1, 2, 4,
            3, 2, 0, 3;

        matcher.setData(data);

        REQUIRE(matcher.dimension() == 3);
        REQUIRE(matcher.size() == 4);

        Matrix points(3, 1);
        points << 0, 1, 0;
        Matrixi indices;
        Matrix distances;

        matcher.query(points, 1, indices, distances);

        REQUIRE(indices.size() == 1);
        REQUIRE(indices(0) == 2);
        REQUIRE(distances.size() == 1);
        REQUIRE(distances(0) == Approx(1.0));
    }

    SECTION("euclidean query multiple")
    {
        BruteForceKNN<Scalar, EuclideanDistance<Scalar>> matcher;

        Matrix data(3, 9);
        data << 1, 2, 3, 1, 2, 3, 1, 2, 1,
                2, 1, 0, 3, 2, 1, 0, 3, 1,
                3, 1, 3, 1, 3, 4, 4, 2, 1;

        matcher.setData(data);

        REQUIRE(matcher.dimension() == 3);
        REQUIRE(matcher.size() == 9);

        Matrix points(3, 1);
        points << 0, 1, 0;

        Matrixi indicesExp(3, 1);
        indicesExp << 8, 1, 3;
        Matrix distancesExp(3, 1);
        distancesExp << std::sqrt(2), std::sqrt(5), std::sqrt(6);

        Matrixi indices;
        Matrix distances;
        matcher.query(points, 3, indices, distances);

        REQUIRE(indices.size() == 3);
        REQUIRE(distances.size() == 3);
        REQUIRE_MATRIX(indicesExp, indices);
        REQUIRE_MATRIX_APPROX(distancesExp, distances, 1e-3);
    }

    SECTION("manhatten query multiple")
    {
        BruteForceKNN<Scalar, ManhattenDistance<Scalar>> matcher;

        Matrix data(3, 9);
        data << 1, 2, 3, 1, 2, 3, 1, 2, 3,
                2, 1, 0, 3, 2, 1, 0, 3, 4,
                3, 1, 3, 1, 3, 4, 4, 2, 1;

        matcher.setData(data);

        REQUIRE(matcher.dimension() == 3);
        REQUIRE(matcher.size() == 9);

        Matrix points(3, 1);
        points << 0, 1, 0;

        Matrixi indicesExp(3, 1);
        indicesExp << 1, 3, 0;
        Matrix distancesExp(3, 1);
        distancesExp << 3, 4, 5;

        Matrixi indices;
        Matrix distances;
        matcher.query(points, 3, indices, distances);

        REQUIRE(indices.size() == 3);
        REQUIRE(distances.size() == 3);
        REQUIRE_MATRIX(indicesExp, indices);
        REQUIRE_MATRIX_APPROX(distancesExp, distances, 1e-3);
    }

    SECTION("hamming distance query multiple")
    {
        BruteForceKNN<Scalar, HammingDistance<uint8_t>, uint8_t> matcher;

        Matrixu8 data(3, 9);
        data << 0x01, 0x03, 0xa3, 0x03, 0xd9, 0xcf, 0x88, 0x8a, 0x13,
                0x00, 0x11, 0xd1, 0x01, 0xcc, 0xad, 0x9a, 0x7d, 0x78,
                0x02, 0x03, 0xff, 0x01, 0x94, 0xf3, 0x22, 0xa6, 0x2f;

        matcher.setData(data);

        REQUIRE(matcher.dimension() == 3);
        REQUIRE(matcher.size() == 9);

        Matrixu8 points(3, 1);
        points << 0x03, 0x13, 0x03;

        Matrixi indicesExp(3, 1);
        indicesExp << 1, 3, 0;
        Matrix distancesExp(3, 1);
        distancesExp << 1, 3, 5;

        Matrixi indices;
        Matrix distances;
        matcher.query(points, 3, indices, distances);

        REQUIRE(indices.size() == 3);
        REQUIRE(distances.size() == 3);
        REQUIRE_MATRIX(indicesExp, indices);
        REQUIRE_MATRIX_APPROX(distancesExp, distances, 1e-3);
    }
}
