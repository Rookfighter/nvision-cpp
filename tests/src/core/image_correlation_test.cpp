/* image_correlation_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#include <eigen_require.h>
#include "cve/src/core/image_correlation.h"

using namespace cve;

TEMPLATE_TEST_CASE("image correlation", "[core]",
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
    Image<TestType> img(3, 3);

    img(0, 0).setConstant(44);
    img(0, 1).setConstant(121);
    img(0, 2).setConstant(14);

    img(1, 0).setConstant(32);
    img(1, 1).setConstant(158);
    img(1, 2).setConstant(101);

    img(2, 0).setConstant(219);
    img(2, 1).setConstant(11);
    img(2, 2).setConstant(82);

    SECTION("1x1 kernel")
    {
        Eigen::Array<float32, 1, 1> kernel;
        kernel << float32{0.5};

        auto expr = image::correlate(img, kernel);
        Image<TestType> result = expr;

        Image<TestType> expected = img;
        for(Index i = 0; i < expected.size(); ++i)
            expected(i) *= 0.5;

        REQUIRE(3 == expr.rows());
        REQUIRE(3 == expr.cols());
        REQUIRE(result.rows() == expr.rows());
        REQUIRE(result.cols() == expr.cols());
        REQUIRE_IMAGE_APPROX(expected, result, 1);
    }

    SECTION("3x3 kernel")
    {
        Eigen::Array<float32, 3, 3> kernel;
        kernel.setConstant(float32{1.0f} / float32{9.0f});

        auto expr = image::correlate(img, kernel);
        Image<TestType> result = expr;

        Image<TestType> expected(3, 3);
        expected.setZero();

        expected(0, 0).setConstant(109);
        expected(0, 1).setConstant(84);
        expected(0, 2).setConstant(121);

        expected(1, 0).setConstant(97);
        expected(1, 1).setConstant(86);
        expected(1, 2).setConstant(86);

        expected(2, 0).setConstant(104);
        expected(2, 1).setConstant(99);
        expected(2, 2).setConstant(104);

        REQUIRE(3 == expr.rows());
        REQUIRE(3 == expr.cols());
        REQUIRE(result.rows() == expr.rows());
        REQUIRE(result.cols() == expr.cols());
        REQUIRE_IMAGE_APPROX(expected, result, 1);
    }
}