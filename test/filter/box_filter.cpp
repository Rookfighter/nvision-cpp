/* box_filter.cpp
 *
 * Author: Fabian Meyer
 * Created On: 28 Jun 2019
 */

#include "assert/eigen_require.h"
#include <cve/filter/box_filter.h>

using namespace cve;

TEST_CASE("box_filter")
{
    const float eps = 1e-3;

    BoxFilter<float> filter(3, 5);

    Image8 img(4, 4, 1);
    img(0, 0, 0) = 0;   img(0, 1, 0) = 255; img(0, 2, 0) = 125; img(0, 3, 0) = 255;
    img(1, 0, 0) = 255; img(1, 1, 0) = 0;   img(1, 2, 0) = 0;   img(1, 3, 0) = 125;
    img(2, 0, 0) = 255; img(2, 1, 0) = 255; img(2, 2, 0) = 255; img(2, 3, 0) = 0;
    img(3, 0, 0) = 125; img(3, 1, 0) = 0;   img(3, 2, 0) = 255; img(3, 3, 0) = 125;

    SECTION("ksize = 3")
    {
        Imagef imgAct;
        Imagef imgExp(4, 4, 1);

        imgExp(0, 0, 0) = 139.132; imgExp(0, 1, 0) = 137.813; imgExp(0, 2, 0) = 134.855; imgExp(0, 3, 0) = 133.536;
        imgExp(1, 0, 0) = 141.942; imgExp(1, 1, 0) = 140.647; imgExp(1, 2, 0) = 137.638; imgExp(1, 3, 0) = 136.343;
        imgExp(2, 0, 0) = 146.089; imgExp(2, 1, 0) = 144.881; imgExp(2, 2, 0) = 141.834; imgExp(2, 3, 0) = 140.626;
        imgExp(3, 0, 0) = 148.899; imgExp(3, 1, 0) = 147.715; imgExp(3, 2, 0) = 144.617; imgExp(3, 3, 0) = 143.433;

        REQUIRE(filter.kernelSize() == 3);

        filter(img, imgAct);
        REQUIRE_IMAGE_APPROX(imgExp, imgAct, eps);
    }


    SECTION("ksize = 5")
    {
        Imagef imgAct;

        Imagef imgExp(4, 4, 1);
        imgExp(0, 0, 0) = 141.266; imgExp(0, 1, 0) = 141.252; imgExp(0, 2, 0) = 141.259; imgExp(0, 3, 0) = 141.239;
        imgExp(1, 0, 0) = 141.239; imgExp(1, 1, 0) = 141.225; imgExp(1, 2, 0) = 141.232; imgExp(1, 3, 0) = 141.211;
        imgExp(2, 0, 0) = 141.287; imgExp(2, 1, 0) = 141.273; imgExp(2, 2, 0) = 141.279; imgExp(2, 3, 0) = 141.259;
        imgExp(3, 0, 0) = 141.253; imgExp(3, 1, 0) = 141.239; imgExp(3, 2, 0) = 141.245; imgExp(3, 3, 0) = 141.225;

        filter.setKernelSize(5);

        REQUIRE(filter.kernelSize() == 5);

        filter(img, imgAct);
        REQUIRE_IMAGE_APPROX(imgExp, imgAct, eps);
    }
}
