/* gauss_filter.cpp
 *
 * Author: Fabian Meyer
 * Created On: 28 Jun 2019
 */

#include "assert/eigen_require.h"
#include <cve/filter/gauss_filter.h>

using namespace cve;

TEST_CASE("gauss_filter")
{
    const float eps = 1e-3;

    GaussFilter<float> filter;

    Image8 img(4, 4, 1);
    img(0, 0, 0) = 0;   img(0, 1, 0) = 255; img(0, 2, 0) = 125; img(0, 3, 0) = 255;
    img(1, 0, 0) = 255; img(1, 1, 0) = 0;   img(1, 2, 0) = 0;   img(1, 3, 0) = 125;
    img(2, 0, 0) = 255; img(2, 1, 0) = 255; img(2, 2, 0) = 255; img(2, 3, 0) = 0;
    img(3, 0, 0) = 125; img(3, 1, 0) = 0;   img(3, 2, 0) = 255; img(3, 3, 0) = 125;

    SECTION("ksize = 3")
    {
        Imagef imgAct;
        Imagef imgExp(4, 4, 1);

        imgExp(0, 0, 0) = 126.318; imgExp(0, 1, 0) = 105.854; imgExp(0, 2, 0) = 107.460; imgExp(0, 3, 0) = 113.987;
        imgExp(1, 0, 0) = 160.261; imgExp(1, 1, 0) = 142.436; imgExp(1, 2, 0) = 120.002; imgExp(1, 3, 0) = 114.188;
        imgExp(2, 0, 0) = 162.285; imgExp(2, 1, 0) = 162.922; imgExp(2, 2, 0) = 134.003; imgExp(2, 3, 0) = 132.427;
        imgExp(3, 0, 0) = 165.297; imgExp(3, 1, 0) = 186.835; imgExp(3, 2, 0) = 169.013; imgExp(3, 3, 0) = 165.297;

        REQUIRE(filter.kernelSize() == 3);

        filter(img, imgAct);
        REQUIRE_IMAGE_APPROX(imgExp, imgAct, eps);
    }


    SECTION("ksize = 5")
    {
        Imagef imgAct;

        Imagef imgExp(4, 4, 1);
        imgExp(0, 0, 0) = 144.223; imgExp(0, 1, 0) = 142.090; imgExp(0, 2, 0) = 131.193; imgExp(0, 3, 0) = 121.756;
        imgExp(1, 0, 0) = 132.659; imgExp(1, 1, 0) = 131.868; imgExp(1, 2, 0) = 130.053; imgExp(1, 3, 0) = 120.854;
        imgExp(2, 0, 0) = 164.485; imgExp(2, 1, 0) = 152.844; imgExp(2, 2, 0) = 153.085; imgExp(2, 3, 0) = 146.220;
        imgExp(3, 0, 0) = 160.132; imgExp(3, 1, 0) = 144.933; imgExp(3, 2, 0) = 146.001; imgExp(3, 3, 0) = 130.620;

        filter.setSigma(2);

        REQUIRE(filter.kernelSize() == 5);

        filter(img, imgAct);
        REQUIRE_IMAGE_APPROX(imgExp, imgAct, eps);
    }


}
