/* orb_feature_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 11 Jul 2019
 */

#include "eigen_require.h"
#include <nvision/src/feature/orb_feature.h>

using namespace nvision;

TEMPLATE_TEST_CASE("ORB feature", "[feature]", nvision::float32, nvision::float64)
{
    using FeatureMatrix = typename ORBFeature<TestType>::FeatureMatrix;

    // constexpr auto eps = static_cast<TestType>(1e-6);

    SECTION("detect lighter corner")
    {
        Image<Grayf> img(7, 7);
        img.setZero();
        img(0, 4).setConstant(255); img(1, 5).setConstant(255); img(2, 6).setConstant(255);
        img(3, 6).setConstant(255); img(4, 6).setConstant(255); img(5, 5).setConstant(255);
        img(6, 4).setConstant(255); img(6, 3).setConstant(255); img(6, 2).setConstant(255);
        img(5, 1).setConstant(255); img(4, 0).setConstant(255); img(3, 0).setConstant(255);

        FeatureMatrix expected(2, 1);
        expected << 3, 3;

        FeatureMatrix actual;
        ORBFeature<TestType> feature;
        feature(img, actual);

        // REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect darker corner")
    {
        Image<Grayf> img(7, 7);
        img.setConstant(Pixel<Grayf>(255));
        img(0, 4).setConstant(0); img(1, 5).setConstant(0); img(2, 6).setConstant(0);
        img(3, 6).setConstant(0); img(4, 6).setConstant(0); img(5, 5).setConstant(0);
        img(6, 4).setConstant(0); img(6, 3).setConstant(0); img(6, 2).setConstant(0);
        img(5, 1).setConstant(0); img(4, 0).setConstant(0); img(3, 0).setConstant(0);

        FeatureMatrix expected(2, 1);
        expected << 3, 3;

        FeatureMatrix actual;
        ORBFeature<TestType> feature;
        feature(img, actual);

        // REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }


    SECTION("detect no corner")
    {
        Image<Grayf> img(7, 7);
        img.setZero();
        img(0, 4).setConstant(255); img(1, 5).setConstant(255); img(2, 6).setConstant(255);
        img(3, 6).setConstant(255); img(4, 6).setConstant(255); img(5, 5).setConstant(255);
        img(6, 4).setConstant(255); img(6, 3).setConstant(255); img(6, 2).setConstant(255);
        img(5, 1).setConstant(255); img(4, 0).setConstant(255);

        FeatureMatrix expected(2, 0);

        FeatureMatrix actual;
        ORBFeature<TestType> feature;
        feature(img, actual);

        // REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect corner across top")
    {
        Image<Grayf> img(7, 7);
        img.setZero();
        img(0, 2).setConstant(255); img(0, 3).setConstant(255);
        img(0, 4).setConstant(255); img(1, 5).setConstant(255); img(2, 6).setConstant(255);
        img(3, 6).setConstant(255); img(4, 6).setConstant(255); img(5, 5).setConstant(255);
        img(6, 4).setConstant(255); img(6, 3).setConstant(255); img(6, 2).setConstant(255);
        img(5, 1).setConstant(255);

        FeatureMatrix expected(2, 1);
        expected << 3, 3;

        FeatureMatrix actual;
        ORBFeature<TestType> feature;
        feature(img, actual);

        // REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

}
