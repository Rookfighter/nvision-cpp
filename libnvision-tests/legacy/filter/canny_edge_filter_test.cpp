
/* canny_filter_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 28 Jun 2019
 */

#include "eigen_require.h"
#include <nvision/src/filter/canny_edge_filter.h>
#include <nvision/src/filter/sobel_filter.h>

using namespace nvision;

TEMPLATE_TEST_CASE("canny filter", "[filter]", Grayf, RGBf, BGRf, RGBAf, BGRAf)
{
    Image<TestType> img(4, 4);
    img(0, 0).setConstant(0);   img(0, 1).setConstant(255); img(0, 2).setConstant(125); img(0, 3).setConstant(255);
    img(1, 0).setConstant(255); img(1, 1).setConstant(0);   img(1, 2).setConstant(0);   img(1, 3).setConstant(125);
    img(2, 0).setConstant(255); img(2, 1).setConstant(255); img(2, 2).setConstant(255); img(2, 3).setConstant(0);
    img(3, 0).setConstant(125); img(3, 1).setConstant(0);   img(3, 2).setConstant(255); img(3, 3).setConstant(125);

    SECTION("detect edges")
    {
        Image<TestType> expected(4, 4);
        expected(0, 0).setConstant(0); expected(0, 1).setConstant(1);  expected(0, 2).setConstant(1); expected(0, 3).setConstant(0);
        expected(1, 0).setConstant(0); expected(1, 1).setConstant(1);  expected(1, 2).setConstant(1);    expected(1, 3).setConstant(0);
        expected(2, 0).setConstant(0); expected(2, 1).setConstant(1);  expected(2, 2).setConstant(1);  expected(2, 3).setConstant(0);
        expected(3, 0).setConstant(0); expected(3, 1).setConstant(1); expected(3, 2).setConstant(1);  expected(3, 3).setConstant(0);

        CannyEdgeFilter<float32> edgeFilter;
        SobelFilter<float32> gradientFilter;

        Image<TestType> gradientX = gradientFilter(img, GradientMode::X{});
        Image<TestType> gradientY = gradientFilter(img, GradientMode::Y{});;

        Image<TestType> actual = edgeFilter(gradientX, gradientY);

        // REQUIRE_IMAGE_APPROX(expected, actual, 1e-3);
    }
}
