/* image_resizing_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#include <catch2/catch.hpp>
#include "nvision/src/core/image_resizing.h"

using namespace nvision;

TEMPLATE_TEST_CASE("image resizing", "[core]",
    Gray,
    RGB,
    BGR,
    RGBA,
    BGRA,
    HSV,
    YCbCr)
{
    Image<TestType> img(4, 4);

    img(0, 0).setConstant(44);
    img(0, 1).setConstant(121);
    img(0, 2).setConstant(14);
    img(0, 3).setConstant(208);

    img(1, 0).setConstant(32);
    img(1, 1).setConstant(158);
    img(1, 2).setConstant(101);
    img(1, 3).setConstant(162);

    img(2, 0).setConstant(219);
    img(2, 1).setConstant(11);
    img(2, 2).setConstant(82);
    img(2, 3).setConstant(35);

    img(3, 0).setConstant(83);
    img(3, 1).setConstant(199);
    img(3, 2).setConstant(217);
    img(3, 3).setConstant(105);

    SECTION("uniform downscale")
    {
        auto expr = image::resize(img, 0.5);
        Image<TestType> result = expr;

        REQUIRE(2 == expr.rows());
        REQUIRE(2 == expr.cols());

        REQUIRE(result.rows() == expr.rows());
        REQUIRE(result.cols() == expr.cols());
    }

    SECTION("uniform upscale")
    {
        auto expr = image::resize(img, 1.5);
        Image<TestType> result = expr;

        REQUIRE(6 == expr.rows());
        REQUIRE(6 == expr.cols());

        REQUIRE(result.rows() == expr.rows());
        REQUIRE(result.cols() == expr.cols());
    }
}
