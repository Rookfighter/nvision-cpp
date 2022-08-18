/* fast_feature_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 11 Jul 2019
 */

#include "eigen_require.h"
#include <cve/src/feature/fast_feature.h>

using namespace cve;

TEMPLATE_TEST_CASE("FAST feature", "[feature]", cve::float32, cve::float64)
{
    using FeatureMatrix = typename FASTFeature<TestType>::FeatureMatrix;

    constexpr auto eps = static_cast<TestType>(1e-6);

    SECTION("detect lighter corner")
    {
        Image<Grayf> img(7, 7);
        img.setZero();
        img(0, 4).setConstant(1); img(1, 5).setConstant(1); img(2, 6).setConstant(1);
        img(3, 6).setConstant(1); img(4, 6).setConstant(1); img(5, 5).setConstant(1);
        img(6, 4).setConstant(1); img(6, 3).setConstant(1); img(6, 2).setConstant(1);
        img(5, 1).setConstant(1); img(4, 0).setConstant(1); img(3, 0).setConstant(1);

        FeatureMatrix expected(2, 1);
        expected << 3, 3;

        FeatureMatrix actual;
        FASTFeature<TestType> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect darker corner")
    {
        Image<Grayf> img(7, 7);
        img.setOnes();
        img(0, 4).setConstant(0); img(1, 5).setConstant(0); img(2, 6).setConstant(0);
        img(3, 6).setConstant(0); img(4, 6).setConstant(0); img(5, 5).setConstant(0);
        img(6, 4).setConstant(0); img(6, 3).setConstant(0); img(6, 2).setConstant(0);
        img(5, 1).setConstant(0); img(4, 0).setConstant(0); img(3, 0).setConstant(0);

        FeatureMatrix expected(2, 1);
        expected << 3, 3;

        FeatureMatrix actual;
        FASTFeature<TestType> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect corner Mode5")
    {
        Image<Grayf> img(5, 5);
        img.setOnes();
        img(0, 1).setConstant(0); img(0, 2).setConstant(0); img(0, 3).setConstant(0);
        img(1, 4).setConstant(0); img(2, 4).setConstant(0); img(3, 4).setConstant(0);
        img(4, 1).setConstant(0); img(4, 2).setConstant(0); img(4, 3).setConstant(0);

        FeatureMatrix expected(2, 1);
        expected << 2, 2;

        FeatureMatrix actual;
        FASTFeature<TestType, cve::FASTMode5> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect corner Mode7")
    {
        Image<Grayf> img(7, 7);
        img.setOnes();
        img(0, 4).setConstant(0); img(1, 5).setConstant(0); img(2, 6).setConstant(0);
        img(3, 6).setConstant(0); img(4, 6).setConstant(0); img(5, 5).setConstant(0);
        img(6, 4).setConstant(0); img(6, 3).setConstant(0); img(6, 2).setConstant(0);
        img(5, 1).setConstant(0); img(4, 0).setConstant(0); img(3, 0).setConstant(0);

        FeatureMatrix expected(2, 1);
        expected << 3, 3;

        FeatureMatrix actual;
        FASTFeature<TestType, cve::FASTMode7> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect corner Mode9")
    {
        Image<Grayf> img(9, 9);
        img.setOnes();

        img(0, 3).setConstant(0); img(0, 4).setConstant(0); img(0, 5).setConstant(0);
        img(1, 6).setConstant(0); img(2, 7).setConstant(0);
        img(3, 8).setConstant(0); img(4, 8).setConstant(0); img(5, 8).setConstant(0);
        img(6, 7).setConstant(0); img(7, 6).setConstant(0);
        img(8, 5).setConstant(0); img(8, 4).setConstant(0); img(8, 3).setConstant(0);
        img(7, 2).setConstant(0); img(6, 1).setConstant(0);

        FeatureMatrix expected(2, 1);
        expected << 4, 4;

        FeatureMatrix actual;
        FASTFeature<TestType, cve::FASTMode9> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect no corner")
    {
        Image<Grayf> img(7, 7);
        img.setZero();
        img(0, 4).setConstant(1); img(1, 5).setConstant(1); img(2, 6).setConstant(1);
        img(3, 6).setConstant(1); img(4, 6).setConstant(1); img(5, 5).setConstant(1);
        img(6, 4).setConstant(1); img(6, 3).setConstant(1); img(6, 2).setConstant(1);
        img(5, 1).setConstant(1); img(4, 0).setConstant(1);

        FeatureMatrix expected(2, 0);

        FeatureMatrix actual;
        FASTFeature<TestType> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect corner across top")
    {
        Image<Grayf> img(7, 7);
        img.setZero();
        img(0, 2).setConstant(1); img(0, 3).setConstant(1);
        img(0, 4).setConstant(1); img(1, 5).setConstant(1); img(2, 6).setConstant(1);
        img(3, 6).setConstant(1); img(4, 6).setConstant(1); img(5, 5).setConstant(1);
        img(6, 4).setConstant(1); img(6, 3).setConstant(1); img(6, 2).setConstant(1);
        img(5, 1).setConstant(1);

        FeatureMatrix expected(2, 1);
        expected << 3, 3;

        FeatureMatrix actual;
        FASTFeature<TestType> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

}
