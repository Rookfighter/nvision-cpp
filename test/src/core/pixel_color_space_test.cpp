/* pixel_color_space_conversion_test.cpp
 *
 */

#include <catch2/catch.hpp>
#include "cve/src/core/pixel.h"

using namespace cve;

TEMPLATE_TEST_CASE("any color space to/from RGB", "[core]",
    Gray,
    RGB,
    BGR,
    RGBA,
    BGRA,
    HSV,
    YCbCr,
    Grayf,
    RGBf,
    BGRf,
    RGBAf,
    BGRAf,
    YCbCrf)
{
    SECTION("to RGBf")
    {
        const Pixel<TestType> pixel(TestType::maximum[0]);
        const Pixel<RGBf> expected(RGBf::maximum[0]);
        REQUIRE_NOTHROW(pixel.template convert<RGBf>());
    }

    SECTION("from RGBf")
    {
        const Pixel<RGBf> pixel(RGBf::maximum[0]);
        const Pixel<TestType> expected(TestType::maximum[0]);
        REQUIRE_NOTHROW(pixel.template convert<TestType>());
    }

    SECTION("to RGB")
    {
        const Pixel<TestType> pixel(TestType::maximum[0]);
        const Pixel<RGB> expected(RGB::maximum[0]);
        REQUIRE_NOTHROW(pixel.template convert<RGB>());
    }

    SECTION("from RGB")
    {
        const Pixel<RGB> pixel(RGB::maximum[0]);
        const Pixel<TestType> expected(TestType::maximum[0]);
        REQUIRE_NOTHROW(pixel.template convert<TestType>());
    }
}

TEST_CASE("pixel color space conversion", "[core]")
{
    SECTION("RGB to Gray")
    {
        const Pixel<RGB> pixel1(122, 36, 209);
        const Pixel<Gray> pixel2 = pixel::convert<RGB, Gray>(pixel1);
        const Pixel<Gray> pixel3 = pixel1.convert<Gray>();

        REQUIRE(81 == pixel2[0]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGB> pixel4(21, 241, 78);
        const Pixel<Gray> pixel5 = pixel::convert<RGB, Gray>(pixel4);
        const Pixel<Gray> pixel6 = pixel4.convert<Gray>();

        REQUIRE(156 == pixel5[0]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGB to RGBA")
    {
        const Pixel<RGB> pixel1(122, 36, 209);
        const Pixel<RGBA> pixel2 = pixel::convert<RGB, RGBA>(pixel1);
        const Pixel<RGBA> pixel3 = pixel1.convert<RGBA>();

        REQUIRE(122 == pixel2[0]);
        REQUIRE(36 == pixel2[1]);
        REQUIRE(209 == pixel2[2]);
        REQUIRE(255 == pixel2[3]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGB> pixel4(21, 241, 78);
        const Pixel<RGBA> pixel5 = pixel::convert<RGB, RGBA>(pixel4);
        const Pixel<RGBA> pixel6 = pixel4.convert<RGBA>();

        REQUIRE(21 == pixel5[0]);
        REQUIRE(241 == pixel5[1]);
        REQUIRE(78 == pixel5[2]);
        REQUIRE(255 == pixel5[3]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGB to BGR")
    {
        const Pixel<RGB> pixel1(122, 36, 209);
        const Pixel<BGR> pixel2 = pixel::convert<RGB, BGR>(pixel1);
        const Pixel<BGR> pixel3 = pixel1.convert<BGR>();

        REQUIRE(122 == pixel2[2]);
        REQUIRE(36 == pixel2[1]);
        REQUIRE(209 == pixel2[0]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGB> pixel4(21, 241, 78);
        const Pixel<BGR> pixel5 = pixel::convert<RGB, BGR>(pixel4);
        const Pixel<BGR> pixel6 = pixel4.convert<BGR>();

        REQUIRE(21 == pixel5[2]);
        REQUIRE(241 == pixel5[1]);
        REQUIRE(78 == pixel5[0]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGB to BGRA")
    {
        const Pixel<RGB> pixel1(122, 36, 209);
        const Pixel<BGRA> pixel2 = pixel::convert<RGB, BGRA>(pixel1);
        const Pixel<BGRA> pixel3 = pixel1.convert<BGRA>();

        REQUIRE(122 == pixel2[2]);
        REQUIRE(36 == pixel2[1]);
        REQUIRE(209 == pixel2[0]);
        REQUIRE(255 == pixel2[3]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGB> pixel4(21, 241, 78);
        const Pixel<BGRA> pixel5 = pixel::convert<RGB, BGRA>(pixel4);
        const Pixel<BGRA> pixel6 = pixel4.convert<BGRA>();

        REQUIRE(21 == pixel5[2]);
        REQUIRE(241 == pixel5[1]);
        REQUIRE(78 == pixel5[0]);
        REQUIRE(255 == pixel5[3]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGB to HSV")
    {
        const Pixel<RGB> pixel1(122, 36, 209);
        const Pixel<HSV> pixel2 = pixel::convert<RGB, HSV>(pixel1);
        const Pixel<HSV> pixel3 = pixel1.convert<HSV>();

        REQUIRE(Approx(0.750f).margin(1e-3f) == pixel2[0]);
        REQUIRE(Approx(0.828f).margin(1e-3f) == pixel2[1]);
        REQUIRE(Approx(0.820f).margin(1e-3f) == pixel2[2]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGB> pixel4(21, 241, 78);
        const Pixel<HSV> pixel5 = pixel::convert<RGB, HSV>(pixel4);
        const Pixel<HSV> pixel6 = pixel4.convert<HSV>();

        REQUIRE(Approx(0.377f).margin(1e-3f) == pixel5[0]);
        REQUIRE(Approx(0.913f).margin(1e-3f) == pixel5[1]);
        REQUIRE(Approx(0.945f).margin(1e-3f) == pixel5[2]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGB to YCbCr")
    {
        const Pixel<RGB> pixel1(122, 36, 209);
        const Pixel<YCbCr> pixel2 = pixel::convert<RGB, YCbCr>(pixel1);
        const Pixel<YCbCr> pixel3 = pixel1.convert<YCbCr>();

        REQUIRE(81 == pixel2[0]);
        REQUIRE(199 == pixel2[1]);
        REQUIRE(156 == pixel2[2]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGB> pixel4(21, 241, 78);
        const Pixel<YCbCr> pixel5 = pixel::convert<RGB, YCbCr>(pixel4);
        const Pixel<YCbCr> pixel6 = pixel4.convert<YCbCr>();

        REQUIRE(156 == pixel5[0]);
        REQUIRE(83 == pixel5[1]);
        REQUIRE(31 == pixel5[2]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("BGR to Gray")
    {
        const Pixel<BGR> pixel1(209, 36, 122);
        const Pixel<Gray> pixel2 = pixel::convert<BGR, Gray>(pixel1);
        const Pixel<Gray> pixel3 = pixel1.convert<Gray>();

        REQUIRE(81 == pixel2[0]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<BGR> pixel4(78, 241, 21);
        const Pixel<Gray> pixel5 = pixel::convert<BGR, Gray>(pixel4);
        const Pixel<Gray> pixel6 = pixel4.convert<Gray>();

        REQUIRE(156 == pixel5[0]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("BGR to BGRA")
    {
        const Pixel<BGR> pixel1(122, 36, 209);
        const Pixel<BGRA> pixel2 = pixel::convert<BGR, BGRA>(pixel1);
        const Pixel<BGRA> pixel3 = pixel1.convert<BGRA>();

        REQUIRE(122 == pixel2[0]);
        REQUIRE(36 == pixel2[1]);
        REQUIRE(209 == pixel2[2]);
        REQUIRE(255 == pixel2[3]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<BGR> pixel4(21, 241, 78);
        const Pixel<BGRA> pixel5 = pixel::convert<BGR, BGRA>(pixel4);
        const Pixel<BGRA> pixel6 = pixel4.convert<BGRA>();

        REQUIRE(21 == pixel5[0]);
        REQUIRE(241 == pixel5[1]);
        REQUIRE(78 == pixel5[2]);
        REQUIRE(255 == pixel5[3]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("BGR to RGBA")
    {
        const Pixel<BGR> pixel1(122, 36, 209);
        const Pixel<RGBA> pixel2 = pixel::convert<BGR, RGBA>(pixel1);
        const Pixel<RGBA> pixel3 = pixel1.convert<RGBA>();

        REQUIRE(122 == pixel2[2]);
        REQUIRE(36 == pixel2[1]);
        REQUIRE(209 == pixel2[0]);
        REQUIRE(255 == pixel2[3]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<BGR> pixel4(21, 241, 78);
        const Pixel<RGBA> pixel5 = pixel::convert<BGR, RGBA>(pixel4);
        const Pixel<RGBA> pixel6 = pixel4.convert<RGBA>();

        REQUIRE(21 == pixel5[2]);
        REQUIRE(241 == pixel5[1]);
        REQUIRE(78 == pixel5[0]);
        REQUIRE(255 == pixel5[3]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("BGR to HSV")
    {
        const Pixel<BGR> pixel1(209, 36, 122);
        const Pixel<HSV> pixel2 = pixel::convert<BGR, HSV>(pixel1);
        const Pixel<HSV> pixel3 = pixel1.convert<HSV>();

        REQUIRE(Approx(0.750f).margin(1e-3f) == pixel2[0]);
        REQUIRE(Approx(0.828f).margin(1e-3f) == pixel2[1]);
        REQUIRE(Approx(0.820f).margin(1e-3f) == pixel2[2]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<BGR> pixel4(78, 241, 21);
        const Pixel<HSV> pixel5 = pixel::convert<BGR, HSV>(pixel4);
        const Pixel<HSV> pixel6 = pixel4.convert<HSV>();

        REQUIRE(Approx(0.377f).margin(1e-3f) == pixel5[0]);
        REQUIRE(Approx(0.913f).margin(1e-3f) == pixel5[1]);
        REQUIRE(Approx(0.945f).margin(1e-3f) == pixel5[2]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("BGR to YCbCr")
    {
        const Pixel<BGR> pixel1(209, 36, 122);
        const Pixel<YCbCr> pixel2 = pixel::convert<BGR, YCbCr>(pixel1);
        const Pixel<YCbCr> pixel3 = pixel1.convert<YCbCr>();

        REQUIRE(81 == pixel2[0]);
        REQUIRE(199 == pixel2[1]);
        REQUIRE(156 == pixel2[2]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<BGR> pixel4(78, 241, 21);
        const Pixel<YCbCr> pixel5 = pixel::convert<BGR, YCbCr>(pixel4);
        const Pixel<YCbCr> pixel6 = pixel4.convert<YCbCr>();

        REQUIRE(156 == pixel5[0]);
        REQUIRE(83 == pixel5[1]);
        REQUIRE(31 == pixel5[2]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGBA to BGRA")
    {
        const Pixel<RGBA> pixel1(122, 36, 209, 157);
        const Pixel<BGRA> pixel2 = pixel::convert<RGBA, BGRA>(pixel1);
        const Pixel<BGRA> pixel3 = pixel1.convert<BGRA>();

        REQUIRE(122 == pixel2[2]);
        REQUIRE(36 == pixel2[1]);
        REQUIRE(209 == pixel2[0]);
        REQUIRE(157 == pixel2[3]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGBA> pixel4(21, 241, 78, 29);
        const Pixel<BGRA> pixel5 = pixel::convert<RGBA, BGRA>(pixel4);
        const Pixel<BGRA> pixel6 = pixel4.convert<BGRA>();

        REQUIRE(21 == pixel5[2]);
        REQUIRE(241 == pixel5[1]);
        REQUIRE(78 == pixel5[0]);
        REQUIRE(29 == pixel5[3]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGBA to Gray")
    {
        const Pixel<RGBA> pixel1(122, 36, 209,132);
        const Pixel<Gray> pixel2 = pixel::convert<RGBA, Gray>(pixel1);
        const Pixel<Gray> pixel3 = pixel1.convert<Gray>();

        REQUIRE(81 == pixel2[0]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGBA> pixel4(21, 241, 78, 44);
        const Pixel<Gray> pixel5 = pixel::convert<RGBA, Gray>(pixel4);
        const Pixel<Gray> pixel6 = pixel4.convert<Gray>();

        REQUIRE(156 == pixel5[0]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGBA to RGB")
    {
        const Pixel<RGBA> pixel1(122, 36, 209, 132);
        const Pixel<RGB> pixel2 = pixel::convert<RGBA, RGB>(pixel1);
        const Pixel<RGB> pixel3 = pixel1.convert<RGB>();

        REQUIRE(122 == pixel2[0]);
        REQUIRE(36 == pixel2[1]);
        REQUIRE(209 == pixel2[2]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGBA> pixel4(21, 241, 78, 44);
        const Pixel<RGB> pixel5 = pixel::convert<RGBA, RGB>(pixel4);
        const Pixel<RGB> pixel6 = pixel4.convert<RGB>();

        REQUIRE(21 == pixel5[0]);
        REQUIRE(241 == pixel5[1]);
        REQUIRE(78 == pixel5[2]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGBA to BGR")
    {
        const Pixel<RGBA> pixel1(122, 36, 209, 132);
        const Pixel<BGR> pixel2 = pixel::convert<RGBA, BGR>(pixel1);
        const Pixel<BGR> pixel3 = pixel1.convert<BGR>();

        REQUIRE(122 == pixel2[2]);
        REQUIRE(36 == pixel2[1]);
        REQUIRE(209 == pixel2[0]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGBA> pixel4(21, 241, 78, 44);
        const Pixel<BGR> pixel5 = pixel::convert<RGBA, BGR>(pixel4);
        const Pixel<BGR> pixel6 = pixel4.convert<BGR>();

        REQUIRE(21 == pixel5[2]);
        REQUIRE(241 == pixel5[1]);
        REQUIRE(78 == pixel5[0]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGBA to BGRA")
    {
        const Pixel<RGBA> pixel1(122, 36, 209, 132);
        const Pixel<BGRA> pixel2 = pixel::convert<RGBA, BGRA>(pixel1);
        const Pixel<BGRA> pixel3 = pixel1.convert<BGRA>();

        REQUIRE(122 == pixel2[2]);
        REQUIRE(36 == pixel2[1]);
        REQUIRE(209 == pixel2[0]);
        REQUIRE(132 == pixel2[3]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGBA> pixel4(21, 241, 78, 44);
        const Pixel<BGRA> pixel5 = pixel::convert<RGBA, BGRA>(pixel4);
        const Pixel<BGRA> pixel6 = pixel4.convert<BGRA>();

        REQUIRE(21 == pixel5[2]);
        REQUIRE(241 == pixel5[1]);
        REQUIRE(78 == pixel5[0]);
        REQUIRE(44 == pixel5[3]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("RGBA to HSV")
    {
        const Pixel<RGBA> pixel1(122, 36, 209, 132);
        const Pixel<HSV> pixel2 = pixel::convert<RGBA, HSV>(pixel1);
        const Pixel<HSV> pixel3 = pixel1.convert<HSV>();

        REQUIRE(Approx(0.750f).margin(1e-3f) == pixel2[0]);
        REQUIRE(Approx(0.828f).margin(1e-3f) == pixel2[1]);
        REQUIRE(Approx(0.820f).margin(1e-3f) == pixel2[2]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<RGBA> pixel4(21, 241, 78, 44);
        const Pixel<HSV> pixel5 = pixel::convert<RGBA, HSV>(pixel4);
        const Pixel<HSV> pixel6 = pixel4.convert<HSV>();

        REQUIRE(Approx(0.377f).margin(1e-3f) == pixel5[0]);
        REQUIRE(Approx(0.913f).margin(1e-3f) == pixel5[1]);
        REQUIRE(Approx(0.945f).margin(1e-3f) == pixel5[2]);
        REQUIRE(pixel5 == pixel6);
    }

    SECTION("BGRA to HSV")
    {
        const Pixel<BGRA> pixel1(209, 36, 122, 132);
        const Pixel<HSV> pixel2 = pixel::convert<BGRA, HSV>(pixel1);
        const Pixel<HSV> pixel3 = pixel1.convert<HSV>();

        REQUIRE(Approx(0.750f).margin(1e-3f) == pixel2[0]);
        REQUIRE(Approx(0.828f).margin(1e-3f) == pixel2[1]);
        REQUIRE(Approx(0.820f).margin(1e-3f) == pixel2[2]);
        REQUIRE(pixel2 == pixel3);

        const Pixel<BGRA> pixel4(78, 241, 21, 44);
        const Pixel<HSV> pixel5 = pixel::convert<BGRA, HSV>(pixel4);
        const Pixel<HSV> pixel6 = pixel4.convert<HSV>();

        REQUIRE(Approx(0.377f).margin(1e-3f) == pixel5[0]);
        REQUIRE(Approx(0.913f).margin(1e-3f) == pixel5[1]);
        REQUIRE(Approx(0.945f).margin(1e-3f) == pixel5[2]);
        REQUIRE(pixel5 == pixel6);
    }

}