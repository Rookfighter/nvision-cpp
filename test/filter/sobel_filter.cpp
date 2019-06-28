
/* box_filter.cpp
 *
 * Author: Fabian Meyer
 * Created On: 28 Jun 2019
 */

#include "assert/eigen_require.h"
#include <cve/filter/sobel_filter.h>

using namespace cve;

TEST_CASE("sobel_filter")
{
    const float eps = 1e-6;

    Image8 img(4, 4, 1);
    img(0, 0, 0) = 0;   img(0, 1, 0) = 255; img(0, 2, 0) = 125; img(0, 3, 0) = 255;
    img(1, 0, 0) = 255; img(1, 1, 0) = 0;   img(1, 2, 0) = 0;   img(1, 3, 0) = 125;
    img(2, 0, 0) = 255; img(2, 1, 0) = 255; img(2, 2, 0) = 255; img(2, 3, 0) = 0;
    img(3, 0, 0) = 125; img(3, 1, 0) = 0;   img(3, 2, 0) = 255; img(3, 3, 0) = 125;

    Imagef gradExpX(4, 4, 1);
    gradExpX(0, 0, 0) = 0; gradExpX(0, 1, 0) = -260;  gradExpX(0, 2, 0) = 250; gradExpX(0, 3, 0) = 0;
    gradExpX(1, 0, 0) = 0; gradExpX(1, 1, 0) = -385;  gradExpX(1, 2, 0) = -5;    gradExpX(1, 3, 0) = 0;
    gradExpX(2, 0, 0) = 0; gradExpX(2, 1, 0) = -125;  gradExpX(2, 2, 0) = -260;  gradExpX(2, 3, 0) = 0;
    gradExpX(3, 0, 0) = 0; gradExpX(3, 1, 0) = 260; gradExpX(3, 2, 0) = -260;  gradExpX(3, 3, 0) = 0;

    Imagef gradExpY(4, 4, 1);
    gradExpY(0, 0, 0) = 0;    gradExpY(0, 1, 0) = 0;   gradExpY(0, 2, 0) = 0;   gradExpY(0, 3, 0) = 0;
    gradExpY(1, 0, 0) = 510;  gradExpY(1, 1, 0) = 385; gradExpY(1, 2, 0) = 5;   gradExpY(1, 3, 0) = -250;
    gradExpY(2, 0, 0) = -260; gradExpY(2, 1, 0) = 125; gradExpY(2, 2, 0) = 510; gradExpY(2, 3, 0) = 510;
    gradExpY(3, 0, 0) = 0;    gradExpY(3, 1, 0) = 0;   gradExpY(3, 2, 0) = 0;   gradExpY(3, 3, 0) = 0;

    SobelFilter<float> filter;

    SECTION("x-derivative")
    {
        Imagef gradAct;

        filter.applyX(img, gradAct);
        REQUIRE_IMAGE_APPROX(gradExpX, gradAct, eps);
    }

    SECTION("y-derivative")
    {
        Imagef gradAct;

        filter.applyY(img, gradAct);
        REQUIRE_IMAGE_APPROX(gradExpY, gradAct, eps);
    }

    SECTION("x- and y-derivative")
    {
        Imagef gradActX;
        Imagef gradActY;

        filter(img, gradActX, gradActY);
        REQUIRE_IMAGE_APPROX(gradExpX, gradActX, eps);
        REQUIRE_IMAGE_APPROX(gradExpY, gradActY, eps);
    }
}
