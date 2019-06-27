/* forward_differences_filter.cpp
 *
 * Author: Fabian Meyer
 * Created On: 27 Jun 2019
 */

#include "assert/eigen_require.h"
#include <cve/filter/forward_differences_filter.h>

using namespace cve;

TEST_CASE("forward_differences_filter")
{
    const float eps = 1e-6;

    Image8 img(4, 4, 1);
    img(0, 0, 0) = 0;   img(0, 1, 0) = 255; img(0, 2, 0) = 125; img(0, 3, 0) = 255;
    img(1, 0, 0) = 255; img(1, 1, 0) = 0;   img(1, 2, 0) = 0;   img(1, 3, 0) = 125;
    img(2, 0, 0) = 255; img(2, 1, 0) = 255; img(2, 2, 0) = 255; img(2, 3, 0) = 0;
    img(3, 0, 0) = 125; img(3, 1, 0) = 0;   img(3, 2, 0) = 255; img(3, 3, 0) = 125;

    ForwardDifferencesFilter<float> filter;

    SECTION("x-derivative")
    {
        Imagef gradAct;
        Imagef gradExp(4, 4, 1);

        gradExp(0, 0, 0) = 255;  gradExp(0, 1, 0) = -130; gradExp(0, 2, 0) = 130;  gradExp(0, 3, 0) = -130;
        gradExp(1, 0, 0) = -255; gradExp(1, 1, 0) = 0;    gradExp(1, 2, 0) = 125;  gradExp(1, 3, 0) = -125;
        gradExp(2, 0, 0) = 0;    gradExp(2, 1, 0) = 0;    gradExp(2, 2, 0) = -255; gradExp(2, 3, 0) = 255;
        gradExp(3, 0, 0) = -125; gradExp(3, 1, 0) = 255;  gradExp(3, 2, 0) = -130; gradExp(3, 3, 0) = 130;

        filter.applyX(img, gradAct);
        REQUIRE_IMAGE_APPROX(gradExp, gradAct, eps);
    }

    SECTION("y-derivative")
    {
        Imagef gradAct;
        Imagef gradExp(4, 4, 1);

        gradExp(0, 0, 0) = 255;  gradExp(0, 1, 0) = -255; gradExp(0, 2, 0) = -125; gradExp(0, 3, 0) = -130;
        gradExp(1, 0, 0) = 0;    gradExp(1, 1, 0) = 255;  gradExp(1, 2, 0) = 255;  gradExp(1, 3, 0) = -125;
        gradExp(2, 0, 0) = -130; gradExp(2, 1, 0) = -255; gradExp(2, 2, 0) = 0;    gradExp(2, 3, 0) = 125;
        gradExp(3, 0, 0) = 130;  gradExp(3, 1, 0) = 255;  gradExp(3, 2, 0) = 0;    gradExp(3, 3, 0) = -125;

        filter.applyY(img, gradAct);
        REQUIRE_IMAGE_APPROX(gradExp, gradAct, eps);
    }
}
