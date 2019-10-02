/* harris_detector.cpp
 *
 * Author: Fabian Meyer
 * Created On: 11 Jul 2019
 */

#include "assert/eigen_require.h"
#include <cve/feature/harris_detector.h>

using namespace cve;

typedef float Scalar;
typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;

TEST_CASE("harris_detector")
{
    Scalar eps = 1e-6;

    HarrisDetector<Scalar> harris;

    SECTION("detect light corner")
    {
        Image8 img(7, 7, 1);
        img.setZero();
        img(0, 0, 0) = 255; img(0, 1, 0) = 255; img(0, 2, 0) = 255;
        img(1, 0, 0) = 255; img(1, 1, 0) = 255; img(1, 2, 0) = 255;
        img(2, 0, 0) = 255; img(2, 1, 0) = 255; img(2, 2, 0) = 255;

        Matrix kpExp(2, 1);
        kpExp << 2, 2;
        Matrix kpAct;

        harris(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }

    SECTION("detect dark corner")
    {
        Image8 img(7, 7, 1);
        img.setConstant(1);
        img *= img.constant(1);
        img(0, 0, 0) = 0; img(0, 1, 0) = 0; img(0, 2, 0) = 0;
        img(1, 0, 0) = 0; img(1, 1, 0) = 0; img(1, 2, 0) = 0;
        img(2, 0, 0) = 0; img(2, 1, 0) = 0; img(2, 2, 0) = 0;

        Matrix kpExp(2, 1);
        kpExp << 2, 2;
        Matrix kpAct;

        harris(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }

    SECTION("detect no corner")
    {
        Image8 img(7, 7, 1);
        img.setZero();

        Matrix kpExp(2, 0);
        Matrix kpAct;

        harris(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }
}
