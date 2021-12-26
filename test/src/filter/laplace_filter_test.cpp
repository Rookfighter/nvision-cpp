
/* laplace_filter_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 28 Jun 2019
 */

#include "eigen_require.h"
#include <cve/src/filter/laplace_filter.h>

using namespace cve;

TEMPLATE_TEST_CASE("laplace filter", "[filter]", Grayf, RGBf, BGRf, RGBAf, BGRAf)
{
    Image<TestType> img(4, 4);
    img(0, 0).setConstant(0);   img(0, 1).setConstant(255); img(0, 2).setConstant(125); img(0, 3).setConstant(255);
    img(1, 0).setConstant(255); img(1, 1).setConstant(0);   img(1, 2).setConstant(0);   img(1, 3).setConstant(125);
    img(2, 0).setConstant(255); img(2, 1).setConstant(255); img(2, 2).setConstant(255); img(2, 3).setConstant(0);
    img(3, 0).setConstant(125); img(3, 1).setConstant(0);   img(3, 2).setConstant(255); img(3, 3).setConstant(125);

    SECTION("derivative")
    {
        Image<TestType> expected(4, 4);
        expected(0, 0).setConstant(1020); expected(0, 1).setConstant(-895); expected(0, 2).setConstant(10);   expected(0, 3).setConstant(-520);
        expected(1, 0).setConstant(-765); expected(1, 1).setConstant(765); expected(1, 2).setConstant(505);  expected(1, 3).setConstant(-245);
        expected(2, 0).setConstant(-130); expected(2, 1).setConstant(-510); expected(2, 2).setConstant(-510); expected(2, 3).setConstant(760);
        expected(3, 0).setConstant(10);   expected(3, 1).setConstant(890);  expected(3, 2).setConstant(-385); expected(3, 3).setConstant(10);

        LaplaceFilter<float> filter;
        Image<TestType> actual = filter(img);

        REQUIRE_IMAGE_APPROX(expected, actual, 1e-3);
    }
}
