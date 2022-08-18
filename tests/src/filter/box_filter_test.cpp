/* box_filter_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 28 Jun 2019
 */

#include "eigen_require.h"
#include <cve/src/filter/box_filter.h>

using namespace cve;

TEMPLATE_TEST_CASE("box filter", "[filter]", Gray, RGB, BGR, RGBA, BGRA, Grayf, RGBf, BGRf, RGBAf, BGRAf)
{
    Image<TestType> img(4, 4);
    img(0, 0).setConstant(0);   img(0, 1).setConstant(255); img(0, 2).setConstant(125); img(0, 3).setConstant(255);
    img(1, 0).setConstant(255); img(1, 1).setConstant(0);   img(1, 2).setConstant(0);   img(1, 3).setConstant(125);
    img(2, 0).setConstant(255); img(2, 1).setConstant(255); img(2, 2).setConstant(255); img(2, 3).setConstant(0);
    img(3, 0).setConstant(125); img(3, 1).setConstant(0);   img(3, 2).setConstant(255); img(3, 3).setConstant(125);

    SECTION("ksize = 3")
    {
        Image<TestType> expected(4, 4);

        expected(0, 0).setConstant(113); expected(0, 1).setConstant(98); expected(0, 2).setConstant(98); expected(0, 3).setConstant(83);
        expected(1, 0).setConstant(170); expected(1, 1).setConstant(155); expected(1, 2).setConstant(141); expected(1, 3).setConstant(126);
        expected(2, 0).setConstant(127); expected(2, 1).setConstant(155); expected(2, 2).setConstant(112); expected(2, 3).setConstant(141);
        expected(3, 0).setConstant(183); expected(3, 1).setConstant(212); expected(3, 2).setConstant(155); expected(3, 3).setConstant(183);

        BoxFilter<float, 3> filter;
        Image<TestType> actual = filter(img);

        REQUIRE_IMAGE_APPROX(expected, actual, 1);
    }


    SECTION("ksize = 5")
    {
        Image<TestType> expected(4, 4);

        expected(0, 0).setConstant(152); expected(0, 1).setConstant(147); expected(0, 2).setConstant(142); expected(0, 3).setConstant(132);
        expected(1, 0).setConstant(127); expected(1, 1).setConstant(126); expected(1, 2).setConstant(131); expected(1, 3).setConstant(116);
        expected(2, 0).setConstant(167); expected(2, 1).setConstant(152); expected(2, 2).setConstant(157); expected(2, 3).setConstant(152);
        expected(3, 0).setConstant(147); expected(3, 1).setConstant(132); expected(3, 2).setConstant(142); expected(3, 3).setConstant(116);

        BoxFilter<float, 5> filter;
        Image<TestType> actual = filter(img);

        REQUIRE_IMAGE_APPROX(expected, actual, 1);
    }
}
