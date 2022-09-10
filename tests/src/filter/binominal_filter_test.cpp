/* binominal_filter_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 28 Jun 2019
 */

#include "eigen_require.h"
#include <nvision/src/filter/binominal_filter.h>

using namespace nvision;

TEMPLATE_TEST_CASE("binominal filter", "[filter]", Gray, RGB, BGR, RGBA, BGRA, Grayf, RGBf, BGRf, RGBAf, BGRAf)
{
    Image<TestType> img(4, 4);
    img(0, 0).setConstant(0);   img(0, 1).setConstant(255); img(0, 2).setConstant(125); img(0, 3).setConstant(255);
    img(1, 0).setConstant(255); img(1, 1).setConstant(0);   img(1, 2).setConstant(0);   img(1, 3).setConstant(125);
    img(2, 0).setConstant(255); img(2, 1).setConstant(255); img(2, 2).setConstant(255); img(2, 3).setConstant(0);
    img(3, 0).setConstant(125); img(3, 1).setConstant(0);   img(3, 2).setConstant(255); img(3, 3).setConstant(125);

    SECTION("ksize = 3")
    {
        Image<TestType> expected(4, 4);

        expected(0, 0).setConstant(127); expected(0, 1).setConstant(111); expected(0, 2).setConstant(110); expected(0, 3).setConstant(126);
        expected(1, 0).setConstant(159); expected(1, 1).setConstant(135); expected(1, 2).setConstant(110); expected(1, 3).setConstant(110);
        expected(2, 0).setConstant(175); expected(2, 1).setConstant(167); expected(2, 2).setConstant(143); expected(2, 3).setConstant(126);
        expected(3, 0).setConstant(158); expected(3, 1).setConstant(175); expected(3, 2).setConstant(175); expected(3, 3).setConstant(158);

        BinominalFilter<float, 3> filter;
        Image<TestType> actual = filter(img);

        REQUIRE_IMAGE_APPROX(expected, actual, 1);
    }

    SECTION("ksize = 5")
    {
        Image<TestType> expected(4, 4);
        expected(0, 0).setConstant(133); expected(0, 1).setConstant(125); expected(0, 2).setConstant(115); expected(0, 3).setConstant(114);
        expected(1, 0).setConstant(146); expected(1, 1).setConstant(137); expected(1, 2).setConstant(123); expected(1, 3).setConstant(118);
        expected(2, 0).setConstant(164); expected(2, 1).setConstant(158); expected(2, 2).setConstant(144); expected(2, 3).setConstant(136);
        expected(3, 0).setConstant(168); expected(3, 1).setConstant(167); expected(3, 2).setConstant(158); expected(3, 3).setConstant(150);

        BinominalFilter<float, 5> filter;
        Image<TestType> actual = filter(img);

        REQUIRE_IMAGE_APPROX(expected, actual, 1);
    }


}
