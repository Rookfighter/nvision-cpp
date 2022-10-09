
/* scharr_filter_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 28 Jun 2019
 */

#include "eigen_require.h"
#include <nvision/src/filter/scharr_filter.h>

using namespace nvision;

TEMPLATE_TEST_CASE("scharr filter", "[filter]", Grayf, RGBf, BGRf, RGBAf, BGRAf)
{
    Image<TestType> img(4, 4);
    img(0, 0).setConstant(0);   img(0, 1).setConstant(255); img(0, 2).setConstant(125); img(0, 3).setConstant(255);
    img(1, 0).setConstant(255); img(1, 1).setConstant(0);   img(1, 2).setConstant(0);   img(1, 3).setConstant(125);
    img(2, 0).setConstant(255); img(2, 1).setConstant(255); img(2, 2).setConstant(255); img(2, 3).setConstant(0);
    img(3, 0).setConstant(125); img(3, 1).setConstant(0);   img(3, 2).setConstant(255); img(3, 3).setConstant(125);

    SECTION("x-derivative")
    {
        Image<TestType> expected(4, 4);
        expected(0, 0).setConstant(0); expected(0, 1).setConstant(-3720);  expected(0, 2).setConstant(11750);  expected(0, 3).setConstant(0);
        expected(1, 0).setConstant(0); expected(1, 1).setConstant(-35435); expected(1, 2).setConstant(8265);   expected(1, 3).setConstant(0);
        expected(2, 0).setConstant(0); expected(2, 1).setConstant(-5875);  expected(2, 2).setConstant(-29560); expected(2, 3).setConstant(0);
        expected(3, 0).setConstant(0); expected(3, 1).setConstant(21060);  expected(3, 2).setConstant(-3720);  expected(3, 3).setConstant(0);

        ScharrFilter<float> filter;
        Image<TestType> actual = filter(img, GradientMode::X());

        REQUIRE_IMAGE_APPROX(expected, actual, 1e-3);
    }

    SECTION("y-derivative")
    {
        Image<TestType> expected(4, 4);
        expected(0, 0).setConstant(0);      expected(0, 1).setConstant(0);     expected(0, 2).setConstant(0);     expected(0, 3).setConstant(0);
        expected(1, 0).setConstant(41310);  expected(1, 1).setConstant(18095); expected(1, 2).setConstant(9075);  expected(1, 3).setConstant(-29090);
        expected(2, 0).setConstant(-21060); expected(2, 1).setConstant(5875);  expected(2, 2).setConstant(41310); expected(2, 3).setConstant(23970);
        expected(3, 0).setConstant(0);      expected(3, 1).setConstant(0);     expected(3, 2).setConstant(0);     expected(3, 3).setConstant(0);

        ScharrFilter<float> filter;
        Image<TestType> actual = filter(img, GradientMode::Y());

        REQUIRE_IMAGE_APPROX(expected, actual, 1e-3);
    }
}
