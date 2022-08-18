/* central_differences_filter_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 27 Jun 2019
 */

#include "eigen_require.h"
#include <cve/src/filter/central_differences_filter.h>

using namespace cve;

TEMPLATE_TEST_CASE("central differences filter", "[filter]", Grayf, RGBf, BGRf, RGBAf, BGRAf)
{
    Image<TestType> img(4, 4);
    img(0, 0).setConstant(0);   img(0, 1).setConstant(255); img(0, 2).setConstant(125); img(0, 3).setConstant(255);
    img(1, 0).setConstant(255); img(1, 1).setConstant(0);   img(1, 2).setConstant(0);   img(1, 3).setConstant(125);
    img(2, 0).setConstant(255); img(2, 1).setConstant(255); img(2, 2).setConstant(255); img(2, 3).setConstant(0);
    img(3, 0).setConstant(125); img(3, 1).setConstant(0);   img(3, 2).setConstant(255); img(3, 3).setConstant(125);


    SECTION("x-derivative")
    {
        Image<TestType> expected(4, 4);
        expected(0, 0).setConstant(0); expected(0, 1).setConstant(125);  expected(0, 2).setConstant(0);    expected(0, 3).setConstant(0);
        expected(1, 0).setConstant(0); expected(1, 1).setConstant(-255); expected(1, 2).setConstant(125);  expected(1, 3).setConstant(0);
        expected(2, 0).setConstant(0); expected(2, 1).setConstant(0);    expected(2, 2).setConstant(-255); expected(2, 3).setConstant(0);
        expected(3, 0).setConstant(0); expected(3, 1).setConstant(130);  expected(3, 2).setConstant(125);  expected(3, 3).setConstant(0);

        CentralDifferencesFilter<float> filter;
        Image<TestType> actual = filter(img, GradientMode::X());

        REQUIRE_IMAGE_APPROX(expected, actual, 1e-3);
    }

    SECTION("y-derivative")
    {
        Image<TestType> expected(4, 4);
        expected(0, 0).setConstant(0);    expected(0, 1).setConstant(0); expected(0, 2).setConstant(0);   expected(0, 3).setConstant(0);
        expected(1, 0).setConstant(255);  expected(1, 1).setConstant(0); expected(1, 2).setConstant(130); expected(1, 3).setConstant(-255);
        expected(2, 0).setConstant(-130); expected(2, 1).setConstant(0); expected(2, 2).setConstant(255); expected(2, 3).setConstant(0);
        expected(3, 0).setConstant(0);    expected(3, 1).setConstant(0); expected(3, 2).setConstant(0);   expected(3, 3).setConstant(0);

        CentralDifferencesFilter<float> filter;
        Image<TestType> actual = filter(img, GradientMode::Y());
        REQUIRE_IMAGE_APPROX(expected, actual, 1e-3);
    }
}
