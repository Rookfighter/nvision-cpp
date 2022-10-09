/* diffusion_filter_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 28 Jun 2019
 */

#include "eigen_require.h"
#include <nvision/src/filter/diffusion_filter.h>

using namespace nvision;

TEMPLATE_TEST_CASE("diffusion filter", "[filter]", Grayf, RGBf, BGRf, RGBAf, BGRAf)
{
    Image<TestType> img(4, 4);
    img(0, 0).setConstant(0);   img(0, 1).setConstant(255); img(0, 2).setConstant(125); img(0, 3).setConstant(255);
    img(1, 0).setConstant(255); img(1, 1).setConstant(0);   img(1, 2).setConstant(0);   img(1, 3).setConstant(125);
    img(2, 0).setConstant(255); img(2, 1).setConstant(255); img(2, 2).setConstant(255); img(2, 3).setConstant(0);
    img(3, 0).setConstant(125); img(3, 1).setConstant(0);   img(3, 2).setConstant(255); img(3, 3).setConstant(125);

    SECTION("apply")
    {
        Image<TestType> expected(4, 4);

        expected(0, 0).setConstant(126); expected(0, 1).setConstant(105); expected(0, 2).setConstant(107); expected(0, 3).setConstant(113);
        expected(1, 0).setConstant(160); expected(1, 1).setConstant(142); expected(1, 2).setConstant(120); expected(1, 3).setConstant(114);
        expected(2, 0).setConstant(162); expected(2, 1).setConstant(162); expected(2, 2).setConstant(134); expected(2, 3).setConstant(132);
        expected(3, 0).setConstant(165); expected(3, 1).setConstant(186); expected(3, 2).setConstant(169); expected(3, 3).setConstant(165);

        DiffusionFilter<float> filter;
        Image<TestType> actual = filter(img);

        // REQUIRE_IMAGE_APPROX(expected, actual, 1);
    }
}
