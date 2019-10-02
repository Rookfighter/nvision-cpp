/* fast_features.cpp
 *
 * Author: Fabian Meyer
 * Created On: 11 Jul 2019
 */

#include "assert/eigen_require.h"
#include <cve/feature/fast_features.h>

using namespace cve;

typedef cve::FASTFeatures<float> Features;
typedef typename Features::Matrix Matrix;

TEST_CASE("fast_features")
{
    float eps = 1e-6;

    Features features;

    SECTION("detect lighter corner")
    {
        Image8 img(7, 7, 1);
        img.setZero();
        img(0, 4, 0) = 255; img(1, 5, 0) = 255; img(2, 6, 0) = 255;
        img(3, 6, 0) = 255; img(4, 6, 0) = 255; img(5, 5, 0) = 255;
        img(6, 4, 0) = 255; img(6, 3, 0) = 255; img(6, 2, 0) = 255;
        img(5, 1, 0) = 255; img(4, 0, 0) = 255; img(3, 0, 0) = 255;

        Matrix kpExp(2, 1);
        kpExp << 3, 3;
        Matrix kpAct;

        features(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }

    SECTION("detect darker corner")
    {
        Image8 img(7, 7, 1);
        img.setConstant(1);
        img *= img.constant(255);
        img(0, 4, 0) = 0; img(1, 5, 0) = 0; img(2, 6, 0) = 0;
        img(3, 6, 0) = 0; img(4, 6, 0) = 0; img(5, 5, 0) = 0;
        img(6, 4, 0) = 0; img(6, 3, 0) = 0; img(6, 2, 0) = 0;
        img(5, 1, 0) = 0; img(4, 0, 0) = 0; img(3, 0, 0) = 0;

        Matrix kpExp(2, 1);
        kpExp << 3, 3;
        Matrix kpAct;

        features(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }

    SECTION("detect corner Mode5")
    {
        cve::FASTFeatures<float, cve::FASTMode5> features2;

        Image8 img(5, 5, 1);
        img.setConstant(1);
        img *= img.constant(255);
        img(0, 1, 0) = 0; img(0, 2, 0) = 0; img(0, 3, 0) = 0;
        img(1, 4, 0) = 0; img(2, 4, 0) = 0; img(3, 4, 0) = 0;
        img(4, 1, 0) = 0; img(4, 2, 0) = 0; img(4, 3, 0) = 0;

        Matrix kpExp(2, 1);
        kpExp << 2, 2;
        Matrix kpAct;

        features2(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }

    SECTION("detect corner Mode7")
    {
        cve::FASTFeatures<float, cve::FASTMode7> features2;

        Image8 img(7, 7, 1);
        img.setConstant(1);
        img *= img.constant(255);
        img(0, 4, 0) = 0; img(1, 5, 0) = 0; img(2, 6, 0) = 0;
        img(3, 6, 0) = 0; img(4, 6, 0) = 0; img(5, 5, 0) = 0;
        img(6, 4, 0) = 0; img(6, 3, 0) = 0; img(6, 2, 0) = 0;
        img(5, 1, 0) = 0; img(4, 0, 0) = 0; img(3, 0, 0) = 0;

        Matrix kpExp(2, 1);
        kpExp << 3, 3;
        Matrix kpAct;

        features2(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }

    SECTION("detect corner Mode9")
    {
        cve::FASTFeatures<float, cve::FASTMode9> features2;

        Image8 img(9, 9, 1);
        img.setConstant(1);
        img *= img.constant(255);
        img(0, 3, 0) = 0; img(0, 4, 0) = 0; img(0, 5, 0) = 0;
        img(1, 6, 0) = 0; img(2, 7, 0) = 0;
        img(3, 8, 0) = 0; img(4, 8, 0) = 0; img(5, 8, 0) = 0;
        img(6, 7, 0) = 0; img(7, 6, 0) = 0;
        img(8, 5, 0) = 0; img(8, 4, 0) = 0; img(8, 3, 0) = 0;
        img(7, 2, 0) = 0; img(6, 1, 0) = 0;

        Matrix kpExp(2, 1);
        kpExp << 4, 4;
        Matrix kpAct;

        features2(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }

    SECTION("detect no corner")
    {
        Image8 img(7, 7, 1);
        img.setZero();
        img(0, 4, 0) = 255; img(1, 5, 0) = 255; img(2, 6, 0) = 255;
        img(3, 6, 0) = 255; img(4, 6, 0) = 255; img(5, 5, 0) = 255;
        img(6, 4, 0) = 255; img(6, 3, 0) = 255; img(6, 2, 0) = 255;
        img(5, 1, 0) = 255; img(4, 0, 0) = 255;

        Matrix kpExp(2, 0);
        Matrix kpAct;

        features(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }

    SECTION("detect corner across top")
    {
        Image8 img(7, 7, 1);
        img.setZero();
        img(0, 2, 0) = 255; img(0, 3, 0) = 255;
        img(0, 4, 0) = 255; img(1, 5, 0) = 255; img(2, 6, 0) = 255;
        img(3, 6, 0) = 255; img(4, 6, 0) = 255; img(5, 5, 0) = 255;
        img(6, 4, 0) = 255; img(6, 3, 0) = 255; img(6, 2, 0) = 255;
        img(5, 1, 0) = 255;

        Matrix kpExp(2, 1);
        kpExp << 3, 3;
        Matrix kpAct;

        features(img, kpAct);

        REQUIRE_MATRIX_APPROX(kpExp, kpAct, eps);
    }

    SECTION("fail on multichannel image")
    {
        Image8 img(7, 7, 2);
        img.setZero();
        Matrix kpAct;

        REQUIRE_THROWS(features(img, kpAct));
    }
}
