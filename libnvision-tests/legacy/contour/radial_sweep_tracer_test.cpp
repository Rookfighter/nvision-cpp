/// @author Fabian Meyer
/// @date 28 Sep 2022
/// @file

#include <eigen_require.h>
#include "nvision/src/contour/radial_sweep_tracer.h"
#include "contour_test_helper.h"

using namespace nvision;


TEMPLATE_TEST_CASE("radial sweep tracer", "[contour]",
    Gray,
    Grayf)
{
    auto image = Image<TestType>(20, 20);
    auto tracer = RadialSweepTracer{};
    SECTION("square")
    {
        auto expected = std::vector<Contour>{makeSquareContour()};
        makeContourImage(image, expected);

        auto actual = std::vector<Contour>{};
        tracer.trace(image, actual);

        checkContours(expected, actual);
    }

    SECTION("circle")
    {
        auto expected = std::vector<Contour>{makeLargeCircleContour({1, 1})};
        makeContourImage(image, expected);

        auto actual = std::vector<Contour>{};
        tracer.trace(image, actual);

        checkContours(expected, actual);
    }

    SECTION("donut")
    {
        auto expected = std::vector<Contour>{makeLargeCircleContour({1, 1}), makeSmallCircleContour({3, 3})};
        makeContourImage(image, expected);

        auto actual = std::vector<Contour>{};
        tracer.trace(image, actual);

        checkContours(expected, actual);
    }
}
