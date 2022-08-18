/* brief_descriptor_test.cpp
 *
 * Author: Fabian Meyer
 * Created On: 06 Aug 2019
 */

#include "eigen_require.h"
#include <nvision/src/feature/brief_descriptor.h>

using namespace nvision;

TEMPLATE_TEST_CASE("BRIEF descriptor", "[feature]", nvision::float32, nvision::float64)
{
    using Scalar = TestType;
    using PatternMatrix = typename BRIEFDescriptor<Scalar>::PatternMatrix;
    using FeatureMatrix = typename BRIEFDescriptor<Scalar>::FeatureMatrix;
    using DescriptorMatrix = typename BRIEFDescriptor<Scalar>::DescriptorMatrix;

    Image<Grayf> img(5, 5);
    img.setZero();
    img(0, 2).setConstant(255); img(0, 4).setConstant(255);
    img(1, 1).setConstant(255); img(1, 3).setConstant(255);
    img(2, 0).setConstant(255); img(2, 2).setConstant(255);
    img(3, 0).setConstant(255); img(3, 4).setConstant(255);
    img(4, 3).setConstant(255); img(4, 4).setConstant(255);

    SECTION("multi bytes, single keypoint")
    {
        PatternMatrix pattern(4, 32);
        pattern <<  4, 4, 1, 2, 4, 1, 2, 4,  4, 1, 2, 4, 4, 1, 2, 4,  4, 4, 1, 2, 4, 1, 2, 4,  4, 1, 2, 4, 4, 1, 2, 4,
                    2, 3, 4, 2, 3, 4, 2, 2,  2, 4, 2, 3, 3, 4, 2, 2,  2, 3, 4, 2, 3, 4, 2, 2,  2, 4, 2, 3, 3, 4, 2, 2,
                    0, 3, 1, 3, 3, 1, 3, 0,  0, 1, 3, 3, 3, 1, 3, 0,  0, 3, 1, 3, 3, 1, 3, 0,  0, 1, 3, 3, 3, 1, 3, 0,
                    3, 2, 3, 0, 2, 3, 0, 3,  3, 3, 0, 2, 2, 3, 0, 3,  3, 2, 3, 0, 2, 3, 0, 3,  3, 3, 0, 2, 2, 3, 0, 3;

        FeatureMatrix keypoints(2, 1);
        keypoints << 0, 0;

        DescriptorMatrix expected(1, 1);
        expected << 0x5a3a5a3a;

        BRIEFDescriptor<Scalar> descriptor;
        descriptor.setPattern(pattern);

        REQUIRE_MATRIX(pattern, descriptor.pattern());

        DescriptorMatrix actual;
        descriptor(img, keypoints, actual);

        REQUIRE_MATRIX(expected, actual);
    }

    SECTION("multibytes, multi keypoints")
    {
        PatternMatrix pattern(4, 32);
        pattern <<  4, 4, 1, 2, 4, 1, 2, 4,  4, 1, 2, 4, 4, 1, 2, 4,  4, 4, 1, 2, 4, 1, 2, 4,  4, 1, 2, 4, 4, 1, 2, 4,
                    2, 3, 4, 2, 3, 4, 2, 2,  2, 4, 2, 3, 3, 4, 2, 2,  2, 3, 4, 2, 3, 4, 2, 2,  2, 4, 2, 3, 3, 4, 2, 2,
                    0, 3, 1, 3, 3, 1, 3, 0,  0, 1, 3, 3, 3, 1, 3, 0,  0, 3, 1, 3, 3, 1, 3, 0,  0, 1, 3, 3, 3, 1, 3, 0,
                    3, 2, 3, 0, 2, 3, 0, 3,  3, 3, 0, 2, 2, 3, 0, 3,  3, 2, 3, 0, 2, 3, 0, 3,  3, 3, 0, 2, 2, 3, 0, 3;

        FeatureMatrix keypoints(2, 2);
        keypoints << 0, 2,
                    0, 0;

        DescriptorMatrix expected(1, 2);
        expected << 0x5a3a5a3a, 0x24442444;

        BRIEFDescriptor<Scalar> descriptor;
        descriptor.setPattern(pattern);

        REQUIRE_MATRIX(pattern, descriptor.pattern());

        DescriptorMatrix actual;
        descriptor(img, keypoints, actual);

        REQUIRE_MATRIX(expected, actual);
    }
}
