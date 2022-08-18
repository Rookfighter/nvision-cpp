/* image_interpolation_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#include <catch2/catch.hpp>
#include "nvision/src/core/image_interpolation.h"

using namespace nvision;

TEMPLATE_TEST_CASE("image interpolation", "[core]",
    Gray,
    RGB,
    BGR,
    RGBA,
    BGRA,
    HSV,
    YCbCr)
{
    using ValueType = typename TestType::ValueType;
    Image<TestType> img(3, 3);

    img(0, 0).setConstant(1);
    img(0, 1).setConstant(2);
    img(0, 2).setConstant(3);

    img(1, 0).setConstant(4);
    img(1, 1).setConstant(5);
    img(1, 2).setConstant(6);

    img(2, 0).setConstant(7);
    img(2, 1).setConstant(8);
    img(2, 2).setConstant(9);

    SECTION("linear vertical")
    {
        const auto pixel1 = image::interpolateLinearVertical(img, 0.5, 1);

        REQUIRE(Pixel<TestType>(static_cast<ValueType>(3.5)) == pixel1);

        const auto pixel2 = image::interpolateLinearVertical(img, 1.5, 2);

        REQUIRE(Pixel<TestType>(static_cast<ValueType>(7.5)) == pixel2);
    }

    SECTION("linear horizontal")
    {
        const auto pixel1 = image::interpolateLinearHorizontal(img, 1, 0.5);

        REQUIRE(Pixel<TestType>(static_cast<ValueType>(4.5)) == pixel1);

        const auto pixel2 = image::interpolateLinearHorizontal(img, 2, 1.5);

        REQUIRE(Pixel<TestType>(static_cast<ValueType>(8.5)) == pixel2);
    }

    SECTION("bilinear")
    {
        const auto pixel1 = image::interpolateBilinear(img, 0.5, 0.5);

        REQUIRE(Pixel<TestType>(3) == pixel1);

        const auto pixel2 = image::interpolateBilinear(img, 1.5, 0.5);

        REQUIRE(Pixel<TestType>(6) == pixel2);
    }
}
