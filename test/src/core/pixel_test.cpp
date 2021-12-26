/* pixel_test.cpp
 *
 */

#include <catch2/catch.hpp>
#include "cve/src/core/pixel.h"

using namespace cve;

TEMPLATE_TEST_CASE("pixel one-dim", "[core]", Gray, Grayf)
{
    using ValueType = typename TestType::ValueType;

    SECTION("default properties")
    {
        Pixel<TestType> pixel;

        REQUIRE(pixel.size() == Pixel<TestType>::Dimension);
        REQUIRE(typeid(typename Pixel<TestType>::ValueType) == typeid(typename TestType::ValueType));
        REQUIRE(Pixel<TestType>::Dimension == TestType::Dimension);
    }

    SECTION("constant value constructor")
    {
        Pixel<TestType> pixel1(23);

        REQUIRE(23 == pixel1[0]);

        Pixel<TestType> pixel2(12);
        REQUIRE(12 == pixel2[0]);
    }

    SECTION("set constant")
    {
        Pixel<TestType> pixel(23);

        REQUIRE(23 == pixel[0]);

        pixel.setConstant(113);
        REQUIRE(113 == pixel[0]);
    }

    SECTION("set zero")
    {
        Pixel<TestType> pixel(23);
        REQUIRE(23 == pixel[0]);

        pixel.setZero();
        REQUIRE(0 == pixel[0]);
    }

    SECTION("set ones")
    {
        Pixel<TestType> pixel(23);

        REQUIRE(23 == pixel[0]);

        pixel.setOnes();
        REQUIRE(1 == pixel[0]);
    }

    SECTION("index operator")
    {
        Pixel<TestType> pixel(1);

        REQUIRE(1 == pixel[0]);

        pixel[0] = 69;
        REQUIRE(69 == pixel[0]);

        const Pixel<TestType> pixel2 = pixel;
        REQUIRE(69 == pixel2[0]);
    }

    SECTION("equality")
    {
        const Pixel<TestType> pixel1(1);
        const Pixel<TestType> pixel2(1);
        const Pixel<TestType> pixel3(3);

        REQUIRE(pixel1 == pixel2);
        REQUIRE(!(pixel1 == pixel3));
        REQUIRE(!(pixel2 == pixel3));
        REQUIRE(pixel1 != pixel3);
        REQUIRE(pixel2 != pixel3);
    }

    SECTION("addition")
    {
        const Pixel<TestType> pixel1(1);
        const Pixel<TestType> pixel2(61);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 += pixel2;

        REQUIRE(62 == pixel3[0]);
        REQUIRE((pixel1 + pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel2;
        pixel4 += 27;

        REQUIRE(88 == pixel4[0]);
        REQUIRE((27 + pixel2) == pixel4);
        REQUIRE((pixel2 + 27) == pixel4);
        REQUIRE((27.0 + pixel2) == pixel4);
        REQUIRE((pixel2 + 27.0) == pixel4);
        REQUIRE((27.0f + pixel2) == pixel4);
        REQUIRE((pixel2 + 27.0f) == pixel4);
        REQUIRE((27L + pixel2) == pixel4);
        REQUIRE((pixel2 + 27L) == pixel4);
        REQUIRE((27UL + pixel2) == pixel4);
        REQUIRE((pixel2 + 27UL) == pixel4);
    }

    SECTION("subtraction")
    {
        const Pixel<TestType> pixel1(61);
        const Pixel<TestType> pixel2(12);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 -= pixel2;

        REQUIRE(49 == pixel3[0]);
        REQUIRE((pixel1 - pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel1;
        pixel4 -= 21;

        REQUIRE(40 == pixel4[0]);
        REQUIRE((pixel1 - 21) == pixel4);
        REQUIRE((pixel1 - 21.0) == pixel4);
        REQUIRE((pixel1 - 21.0f) == pixel4);
        REQUIRE((pixel1 - 21L) == pixel4);
        REQUIRE((pixel1 - 21UL) == pixel4);

        Pixel<TestType> pixel5 = 83 - pixel2;

        REQUIRE(71 == pixel5[0]);
        REQUIRE((83 - pixel2) == pixel5);
        REQUIRE((83.0 - pixel2) == pixel5);
        REQUIRE((83.0f - pixel2) == pixel5);
        REQUIRE((83L - pixel2) == pixel5);
        REQUIRE((83UL - pixel2) == pixel5);
    }

    SECTION("multiplication")
    {
        const Pixel<TestType> pixel1(3);
        const Pixel<TestType> pixel2(61);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 *= pixel2;

        REQUIRE(183 == pixel3[0]);
        REQUIRE((pixel1 * pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel2;
        pixel4 *= 2;

        REQUIRE(122 == pixel4[0]);
        REQUIRE((2 * pixel2) == pixel4);
        REQUIRE((pixel2 * 2) == pixel4);
        REQUIRE((2.0 * pixel2) == pixel4);
        REQUIRE((pixel2 * 2.0) == pixel4);
        REQUIRE((2.0f * pixel2) == pixel4);
        REQUIRE((pixel2 * 2.0f) == pixel4);
        REQUIRE((2L * pixel2) == pixel4);
        REQUIRE((pixel2 * 2L) == pixel4);
        REQUIRE((2UL * pixel2) == pixel4);
        REQUIRE((pixel2 * 2UL) == pixel4);

        Pixel<TestType> pixel5 = pixel1;
        pixel5 *= 2.5;

        REQUIRE(static_cast<ValueType>(7.5) == pixel5[0]);
    }

    SECTION("division")
    {
        const Pixel<TestType> pixel1(36);
        const Pixel<TestType> pixel2(12);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 /= pixel2;

        REQUIRE(3 == pixel3[0]);
        REQUIRE((pixel1 / pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel1;
        pixel4 /= 3;

        REQUIRE(12 == pixel4[0]);
        REQUIRE((pixel1 / 3) == pixel4);
        REQUIRE((pixel1 / 3.0) == pixel4);
        REQUIRE((pixel1 / 3.0f) == pixel4);
        REQUIRE((pixel1 / 3L) == pixel4);
        REQUIRE((pixel1 / 3UL) == pixel4);

        Pixel<TestType> pixel5 = 108 / pixel2;

        REQUIRE(9 == pixel5[0]);
        REQUIRE((108 / pixel2) == pixel5);
        REQUIRE((108.0 / pixel2) == pixel5);
        REQUIRE((108.0f / pixel2) == pixel5);
        REQUIRE((108L / pixel2) == pixel5);
        REQUIRE((108UL / pixel2) == pixel5);
    }
}

TEMPLATE_TEST_CASE("pixel three-dim", "[core]", RGB, BGR, HSV, YCbCr, RGBf, BGRf, YCbCrf)
{
    using ValueType = typename TestType::ValueType;

    SECTION("default properties")
    {
        Pixel<TestType> pixel;

        REQUIRE(pixel.size() == Pixel<TestType>::Dimension);
        REQUIRE(typeid(typename Pixel<TestType>::ValueType) == typeid(typename TestType::ValueType));
        REQUIRE(Pixel<TestType>::Dimension == TestType::Dimension);
    }

    SECTION("constant value constructor")
    {
        Pixel<TestType> pixel1(23);

        REQUIRE(23 == pixel1[0]);
        REQUIRE(23 == pixel1[1]);
        REQUIRE(23 == pixel1[2]);

        Pixel<TestType> pixel2(12);
        REQUIRE(12 == pixel2[0]);
        REQUIRE(12 == pixel2[1]);
        REQUIRE(12 == pixel2[2]);
    }

    SECTION("value constructor")
    {
        Pixel<TestType> pixel1(1,2,3);

        REQUIRE(1 == pixel1[0]);
        REQUIRE(2 == pixel1[1]);
        REQUIRE(3 == pixel1[2]);

        Pixel<TestType> pixel2(61, 27, 109);
        REQUIRE(61 == pixel2[0]);
        REQUIRE(27 == pixel2[1]);
        REQUIRE(109 == pixel2[2]);
    }

    SECTION("set constant")
    {
        Pixel<TestType> pixel(23);

        REQUIRE(23 == pixel[0]);
        REQUIRE(23 == pixel[1]);
        REQUIRE(23 == pixel[2]);

        pixel.setConstant(113);
        REQUIRE(113 == pixel[0]);
        REQUIRE(113 == pixel[1]);
        REQUIRE(113 == pixel[2]);
    }

    SECTION("set zero")
    {
        Pixel<TestType> pixel(23);

        REQUIRE(23 == pixel[0]);
        REQUIRE(23 == pixel[1]);
        REQUIRE(23 == pixel[2]);

        pixel.setZero();
        REQUIRE(0 == pixel[0]);
        REQUIRE(0 == pixel[1]);
        REQUIRE(0 == pixel[2]);
    }

    SECTION("set ones")
    {
        Pixel<TestType> pixel(23);

        REQUIRE(23 == pixel[0]);
        REQUIRE(23 == pixel[1]);
        REQUIRE(23 == pixel[2]);

        pixel.setOnes();
        REQUIRE(1 == pixel[0]);
        REQUIRE(1 == pixel[1]);
        REQUIRE(1 == pixel[2]);
    }

    SECTION("index operator")
    {
        Pixel<TestType> pixel(1, 2, 3);

        REQUIRE(1 == pixel[0]);
        REQUIRE(2 == pixel[1]);
        REQUIRE(3 == pixel[2]);

        pixel[0] = 69;
        pixel[1] = 22;
        pixel[2] = 187;
        REQUIRE(69 == pixel[0]);
        REQUIRE(22 == pixel[1]);
        REQUIRE(187 == pixel[2]);

        const Pixel<TestType> pixel2 = pixel;
        REQUIRE(69 == pixel2[0]);
        REQUIRE(22 == pixel2[1]);
        REQUIRE(187 == pixel2[2]);
    }

    SECTION("equality")
    {
        const Pixel<TestType> pixel1(1, 2, 3);
        const Pixel<TestType> pixel2(1, 2, 3);
        const Pixel<TestType> pixel3(3, 2, 1);

        REQUIRE(pixel1 == pixel2);
        REQUIRE(!(pixel1 == pixel3));
        REQUIRE(!(pixel2 == pixel3));
        REQUIRE(pixel1 != pixel3);
        REQUIRE(pixel2 != pixel3);
    }

    SECTION("addition")
    {
        const Pixel<TestType> pixel1(1, 2, 3);
        const Pixel<TestType> pixel2(61, 22, 109);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 += pixel2;

        REQUIRE(62 == pixel3[0]);
        REQUIRE(24 == pixel3[1]);
        REQUIRE(112 == pixel3[2]);
        REQUIRE((pixel1 + pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel2;
        pixel4 += 27;

        REQUIRE(88 == pixel4[0]);
        REQUIRE(49 == pixel4[1]);
        REQUIRE(136 == pixel4[2]);
        REQUIRE((27 + pixel2) == pixel4);
        REQUIRE((pixel2 + 27) == pixel4);
        REQUIRE((27.0 + pixel2) == pixel4);
        REQUIRE((pixel2 + 27.0) == pixel4);
        REQUIRE((27.0f + pixel2) == pixel4);
        REQUIRE((pixel2 + 27.0f) == pixel4);
        REQUIRE((27L + pixel2) == pixel4);
        REQUIRE((pixel2 + 27L) == pixel4);
        REQUIRE((27UL + pixel2) == pixel4);
        REQUIRE((pixel2 + 27UL) == pixel4);
    }

    SECTION("subtraction")
    {
        const Pixel<TestType> pixel1(61, 22, 109);
        const Pixel<TestType> pixel2(12, 10, 33);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 -= pixel2;

        REQUIRE(49 == pixel3[0]);
        REQUIRE(12 == pixel3[1]);
        REQUIRE(76 == pixel3[2]);
        REQUIRE((pixel1 - pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel1;
        pixel4 -= 21;

        REQUIRE(40 == pixel4[0]);
        REQUIRE(1 == pixel4[1]);
        REQUIRE(88 == pixel4[2]);
        REQUIRE((pixel1 - 21) == pixel4);
        REQUIRE((pixel1 - 21.0) == pixel4);
        REQUIRE((pixel1 - 21.0f) == pixel4);
        REQUIRE((pixel1 - 21L) == pixel4);
        REQUIRE((pixel1 - 21UL) == pixel4);

        Pixel<TestType> pixel5 = 83 - pixel2;

        REQUIRE(71 == pixel5[0]);
        REQUIRE(73 == pixel5[1]);
        REQUIRE(50 == pixel5[2]);
        REQUIRE((83 - pixel2) == pixel5);
        REQUIRE((83.0 - pixel2) == pixel5);
        REQUIRE((83.0f - pixel2) == pixel5);
        REQUIRE((83L - pixel2) == pixel5);
        REQUIRE((83UL - pixel2) == pixel5);
    }

    SECTION("multiplication")
    {
        const Pixel<TestType> pixel1(3, 2, 1);
        const Pixel<TestType> pixel2(61, 27, 109);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 *= pixel2;

        REQUIRE(183 == pixel3[0]);
        REQUIRE(54 == pixel3[1]);
        REQUIRE(109 == pixel3[2]);
        REQUIRE((pixel1 * pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel2;
        pixel4 *= 2;

        REQUIRE(122 == pixel4[0]);
        REQUIRE(54 == pixel4[1]);
        REQUIRE(218 == pixel4[2]);
        REQUIRE((2 * pixel2) == pixel4);
        REQUIRE((pixel2 * 2) == pixel4);
        REQUIRE((2.0 * pixel2) == pixel4);
        REQUIRE((pixel2 * 2.0) == pixel4);
        REQUIRE((2.0f * pixel2) == pixel4);
        REQUIRE((pixel2 * 2.0f) == pixel4);
        REQUIRE((2L * pixel2) == pixel4);
        REQUIRE((pixel2 * 2L) == pixel4);
        REQUIRE((2UL * pixel2) == pixel4);
        REQUIRE((pixel2 * 2UL) == pixel4);

        Pixel<TestType> pixel5 = pixel1;
        pixel5 *= 2.5;

        REQUIRE(static_cast<ValueType>(7.5) == pixel5[0]);
        REQUIRE(static_cast<ValueType>(5) == pixel5[1]);
        REQUIRE(static_cast<ValueType>(2.5) == pixel5[2]);
    }

    SECTION("division")
    {
        const Pixel<TestType> pixel1(36, 48, 198);
        const Pixel<TestType> pixel2(12, 3, 9);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 /= pixel2;

        REQUIRE(3 == pixel3[0]);
        REQUIRE(16 == pixel3[1]);
        REQUIRE(22 == pixel3[2]);
        REQUIRE((pixel1 / pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel1;
        pixel4 /= 3;

        REQUIRE(12 == pixel4[0]);
        REQUIRE(16 == pixel4[1]);
        REQUIRE(66 == pixel4[2]);
        REQUIRE((pixel1 / 3) == pixel4);
        REQUIRE((pixel1 / 3.0) == pixel4);
        REQUIRE((pixel1 / 3.0f) == pixel4);
        REQUIRE((pixel1 / 3L) == pixel4);
        REQUIRE((pixel1 / 3UL) == pixel4);

        Pixel<TestType> pixel5 = 108 / pixel2;

        REQUIRE(9 == pixel5[0]);
        REQUIRE(36 == pixel5[1]);
        REQUIRE(12 == pixel5[2]);
        REQUIRE((108 / pixel2) == pixel5);
        REQUIRE((108.0 / pixel2) == pixel5);
        REQUIRE((108.0f / pixel2) == pixel5);
        REQUIRE((108L / pixel2) == pixel5);
        REQUIRE((108UL / pixel2) == pixel5);
    }
}

TEMPLATE_TEST_CASE("pixel four-dim", "[core]", RGBA, BGRA, RGBAf, BGRAf)
{
    using ValueType = typename TestType::ValueType;

    SECTION("default properties")
    {
        Pixel<TestType> pixel;

        REQUIRE(pixel.size() == Pixel<TestType>::Dimension);
        REQUIRE(typeid(typename Pixel<TestType>::ValueType) == typeid(typename TestType::ValueType));
        REQUIRE(Pixel<TestType>::Dimension == TestType::Dimension);
    }

    SECTION("constant value constructor")
    {
        Pixel<TestType> pixel1(23);

        REQUIRE(23 == pixel1[0]);
        REQUIRE(23 == pixel1[1]);
        REQUIRE(23 == pixel1[2]);
        REQUIRE(23 == pixel1[3]);

        Pixel<TestType> pixel2(12);
        REQUIRE(12 == pixel2[0]);
        REQUIRE(12 == pixel2[1]);
        REQUIRE(12 == pixel2[2]);
        REQUIRE(12 == pixel2[3]);
    }

    SECTION("value constructor")
    {
        Pixel<TestType> pixel1(1,2,3, 4);

        REQUIRE(1 == pixel1[0]);
        REQUIRE(2 == pixel1[1]);
        REQUIRE(3 == pixel1[2]);
        REQUIRE(4 == pixel1[3]);

        Pixel<TestType> pixel2(61, 27, 109, 7);
        REQUIRE(61 == pixel2[0]);
        REQUIRE(27 == pixel2[1]);
        REQUIRE(109 == pixel2[2]);
        REQUIRE(7 == pixel2[3]);
    }

    SECTION("set constant")
    {
        Pixel<TestType> pixel(23);

        REQUIRE(23 == pixel[0]);
        REQUIRE(23 == pixel[1]);
        REQUIRE(23 == pixel[2]);
        REQUIRE(23 == pixel[3]);

        pixel.setConstant(113);
        REQUIRE(113 == pixel[0]);
        REQUIRE(113 == pixel[1]);
        REQUIRE(113 == pixel[2]);
        REQUIRE(113 == pixel[3]);
    }

    SECTION("set zero")
    {
        Pixel<TestType> pixel(23);

        REQUIRE(23 == pixel[0]);
        REQUIRE(23 == pixel[1]);
        REQUIRE(23 == pixel[2]);
        REQUIRE(23 == pixel[3]);

        pixel.setZero();
        REQUIRE(0 == pixel[0]);
        REQUIRE(0 == pixel[1]);
        REQUIRE(0 == pixel[2]);
        REQUIRE(0 == pixel[3]);
    }

    SECTION("set ones")
    {
        Pixel<TestType> pixel(23);

        REQUIRE(23 == pixel[0]);
        REQUIRE(23 == pixel[1]);
        REQUIRE(23 == pixel[2]);
        REQUIRE(23 == pixel[3]);

        pixel.setOnes();
        REQUIRE(1 == pixel[0]);
        REQUIRE(1 == pixel[1]);
        REQUIRE(1 == pixel[2]);
        REQUIRE(1 == pixel[3]);
    }

    SECTION("index operator")
    {
        Pixel<TestType> pixel(1, 2, 3, 4);

        REQUIRE(1 == pixel[0]);
        REQUIRE(2 == pixel[1]);
        REQUIRE(3 == pixel[2]);
        REQUIRE(4 == pixel[3]);

        pixel[0] = 69;
        pixel[1] = 22;
        pixel[2] = 187;
        pixel[3] = 7;
        REQUIRE(69 == pixel[0]);
        REQUIRE(22 == pixel[1]);
        REQUIRE(187 == pixel[2]);
        REQUIRE(7 == pixel[3]);

        const Pixel<TestType> pixel2 = pixel;
        REQUIRE(69 == pixel2[0]);
        REQUIRE(22 == pixel2[1]);
        REQUIRE(187 == pixel2[2]);
        REQUIRE(7 == pixel2[3]);
    }

    SECTION("equality")
    {
        const Pixel<TestType> pixel1(1, 2, 3, 4);
        const Pixel<TestType> pixel2(1, 2, 3, 4);
        const Pixel<TestType> pixel3(4, 3, 2, 1);

        REQUIRE(pixel1 == pixel2);
        REQUIRE(!(pixel1 == pixel3));
        REQUIRE(!(pixel2 == pixel3));
        REQUIRE(pixel1 != pixel3);
        REQUIRE(pixel2 != pixel3);
    }

    SECTION("addition")
    {
        const Pixel<TestType> pixel1(1, 2, 3, 4);
        const Pixel<TestType> pixel2(61, 22, 109, 7);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 += pixel2;

        REQUIRE(62 == pixel3[0]);
        REQUIRE(24 == pixel3[1]);
        REQUIRE(112 == pixel3[2]);
        REQUIRE(11 == pixel3[3]);
        REQUIRE((pixel1 + pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel2;
        pixel4 += 27;

        REQUIRE(88 == pixel4[0]);
        REQUIRE(49 == pixel4[1]);
        REQUIRE(136 == pixel4[2]);
        REQUIRE(34 == pixel4[3]);
        REQUIRE((27 + pixel2) == pixel4);
        REQUIRE((pixel2 + 27) == pixel4);
        REQUIRE((27.0 + pixel2) == pixel4);
        REQUIRE((pixel2 + 27.0) == pixel4);
        REQUIRE((27.0f + pixel2) == pixel4);
        REQUIRE((pixel2 + 27.0f) == pixel4);
        REQUIRE((27L + pixel2) == pixel4);
        REQUIRE((pixel2 + 27L) == pixel4);
        REQUIRE((27UL + pixel2) == pixel4);
        REQUIRE((pixel2 + 27UL) == pixel4);
    }

    SECTION("subtraction")
    {
        const Pixel<TestType> pixel1(61, 22, 109, 218);
        const Pixel<TestType> pixel2(12, 10, 33, 62);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 -= pixel2;

        REQUIRE(49 == pixel3[0]);
        REQUIRE(12 == pixel3[1]);
        REQUIRE(76 == pixel3[2]);
        REQUIRE(156 == pixel3[3]);
        REQUIRE((pixel1 - pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel1;
        pixel4 -= 21;

        REQUIRE(40 == pixel4[0]);
        REQUIRE(1 == pixel4[1]);
        REQUIRE(88 == pixel4[2]);
        REQUIRE(197 == pixel4[3]);
        REQUIRE((pixel1 - 21) == pixel4);
        REQUIRE((pixel1 - 21.0) == pixel4);
        REQUIRE((pixel1 - 21.0f) == pixel4);
        REQUIRE((pixel1 - 21L) == pixel4);
        REQUIRE((pixel1 - 21UL) == pixel4);

        Pixel<TestType> pixel5 = 83 - pixel2;

        REQUIRE(71 == pixel5[0]);
        REQUIRE(73 == pixel5[1]);
        REQUIRE(50 == pixel5[2]);
        REQUIRE(21 == pixel5[3]);
        REQUIRE((83 - pixel2) == pixel5);
        REQUIRE((83.0 - pixel2) == pixel5);
        REQUIRE((83.0f - pixel2) == pixel5);
        REQUIRE((83L - pixel2) == pixel5);
        REQUIRE((83UL - pixel2) == pixel5);
    }

    SECTION("multiplication")
    {
        const Pixel<TestType> pixel1(3, 2, 1, 4);
        const Pixel<TestType> pixel2(61, 27, 109, 17);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 *= pixel2;

        REQUIRE(183 == pixel3[0]);
        REQUIRE(54 == pixel3[1]);
        REQUIRE(109 == pixel3[2]);
        REQUIRE(68 == pixel3[3]);
        REQUIRE((pixel1 * pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel2;
        pixel4 *= 2;

        REQUIRE(122 == pixel4[0]);
        REQUIRE(54 == pixel4[1]);
        REQUIRE(218 == pixel4[2]);
        REQUIRE(34 == pixel4[3]);
        REQUIRE((2 * pixel2) == pixel4);
        REQUIRE((pixel2 * 2) == pixel4);
        REQUIRE((2.0 * pixel2) == pixel4);
        REQUIRE((pixel2 * 2.0) == pixel4);
        REQUIRE((2.0f * pixel2) == pixel4);
        REQUIRE((pixel2 * 2.0f) == pixel4);
        REQUIRE((2L * pixel2) == pixel4);
        REQUIRE((pixel2 * 2L) == pixel4);
        REQUIRE((2UL * pixel2) == pixel4);
        REQUIRE((pixel2 * 2UL) == pixel4);

        Pixel<TestType> pixel5 = pixel1;
        pixel5 *= 2.5;

        REQUIRE(static_cast<ValueType>(7.5) == pixel5[0]);
        REQUIRE(static_cast<ValueType>(5) == pixel5[1]);
        REQUIRE(static_cast<ValueType>(2.5) == pixel5[2]);
        REQUIRE(static_cast<ValueType>(10) == pixel5[3]);
    }

    SECTION("division")
    {
        const Pixel<TestType> pixel1(36, 48, 198, 48);
        const Pixel<TestType> pixel2(12, 3, 9, 6);

        Pixel<TestType> pixel3 = pixel1;
        pixel3 /= pixel2;

        REQUIRE(3 == pixel3[0]);
        REQUIRE(16 == pixel3[1]);
        REQUIRE(22 == pixel3[2]);
        REQUIRE(8 == pixel3[3]);
        REQUIRE((pixel1 / pixel2) == pixel3);

        Pixel<TestType> pixel4 = pixel1;
        pixel4 /= 3;

        REQUIRE(12 == pixel4[0]);
        REQUIRE(16 == pixel4[1]);
        REQUIRE(66 == pixel4[2]);
        REQUIRE(16 == pixel4[3]);
        REQUIRE((pixel1 / 3) == pixel4);
        REQUIRE((pixel1 / 3.0) == pixel4);
        REQUIRE((pixel1 / 3.0f) == pixel4);
        REQUIRE((pixel1 / 3L) == pixel4);
        REQUIRE((pixel1 / 3UL) == pixel4);

        Pixel<TestType> pixel5 = 108 / pixel2;

        REQUIRE(9 == pixel5[0]);
        REQUIRE(36 == pixel5[1]);
        REQUIRE(12 == pixel5[2]);
        REQUIRE(18 == pixel5[3]);
        REQUIRE((108 / pixel2) == pixel5);
        REQUIRE((108.0 / pixel2) == pixel5);
        REQUIRE((108.0f / pixel2) == pixel5);
        REQUIRE((108L / pixel2) == pixel5);
        REQUIRE((108UL / pixel2) == pixel5);
    }
}