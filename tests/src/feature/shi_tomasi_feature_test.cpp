/* shi_tomasi_feature_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 11 Jul 2019
 */

#include "eigen_require.h"
#include "corner_generator.h"
#include <cve/src/feature/shi_tomasi_feature.h>

using namespace cve;

TEMPLATE_TEST_CASE("shi tomasi feature", "[feature]", cve::float32, cve::float64)
{
    using Scalar = TestType;
    using Vector2 = Eigen::Matrix<Scalar, 2, 1>;
    using FeatureMatrix = typename ShiTomasiFeature<Scalar>::FeatureMatrix;
    using CornerGenerator = test::CornerGenerator<Scalar>;
    using CornerDefinition = test::CornerDefinition<Scalar>;

    constexpr auto eps = static_cast<Scalar>(1e-6);

    SECTION("detect light corner")
    {
        Image<Grayf> img(7, 7);
        img.setZero();
        img(0, 0).setConstant(1); img(0, 1).setConstant(1); img(0, 2).setConstant(1);
        img(1, 0).setConstant(1); img(1, 1).setConstant(1); img(1, 2).setConstant(1);
        img(2, 0).setConstant(1); img(2, 1).setConstant(1); img(2, 2).setConstant(1);

        FeatureMatrix expected(2, 1);
        expected << 2, 2;

        FeatureMatrix actual;
        ShiTomasiFeature<Scalar> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect dark corner")
    {
        Image<Grayf> img(7, 7);
        img.setConstant(Pixel<Grayf>(1));
        img(0, 0).setConstant(0); img(0, 1).setConstant(0); img(0, 2).setConstant(0);
        img(1, 0).setConstant(0); img(1, 1).setConstant(0); img(1, 2).setConstant(0);
        img(2, 0).setConstant(0); img(2, 1).setConstant(0); img(2, 2).setConstant(0);

        FeatureMatrix expected(2, 1);
        expected << 2, 2;

        FeatureMatrix actual;
        ShiTomasiFeature<Scalar> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("detect no corner")
    {
        Image<Grayf> img(7, 7);
        img.setZero();

        FeatureMatrix expected(2, 0);
        FeatureMatrix actual;
        ShiTomasiFeature<Scalar> feature;
        feature(img, actual);

        REQUIRE_MATRIX_APPROX(expected, actual, eps);
    }

    SECTION("generated")
    {
        SECTION("simple")
        {
            constexpr auto maxDist = static_cast<Scalar>(2.5);

            auto generator = CornerGenerator({
                CornerDefinition{Vector2(100, 100), 100, 0},
                CornerDefinition{Vector2(300, 400), 60, 45}
            });

            const auto color = color::black<Grayf>();
            Image<Grayf> img(600, 400);
            img.setOnes();
            FeatureMatrix expected;

            generator.generate(img, expected, color);

            FeatureMatrix actual;
            ShiTomasiFeature<Scalar> feature;
            feature(img, actual);

            REQUIRE(expected.rows() == actual.rows());
            REQUIRE(expected.cols() == actual.cols());

            for(Index c1 = 0; c1 < expected.cols(); ++c1)
            {
                Scalar dist = 9999;
                for(Index c2 = 0; c2 < actual.cols(); ++c2)
                {
                    const auto d = (expected.col(c1) - actual.col(c2)).norm();
                    dist = std::min(dist, d);
                }

                REQUIRE(Approx(Scalar{0}).margin(maxDist) == dist);
            }
        }
    }
}
