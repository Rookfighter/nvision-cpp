/* filter_smooth.cpp
 *
 * Author: Fabian Meyer
 * Created On: 02 Mar 2019
 */

#include <iostream>
#include <cve/filter/gauss_filter.h>
#include <cve/filter/box_filter.h>
#include <cve/filter/recursive_blur_filter.h>
#include <cve/imageio/imageio.h>

using namespace cve;

int main(int argc, const char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: filter_smooth <filename>" << std::endl;
        return -1;
    }

    std::cout << "Load \"" << argv[1] << "\"" << std::endl;
    Image8 img;
    Image8 oimg;
    cve::imload(argv[1], img);
    std::string ext = cve::extension(argv[1]);

    std::cout << "Apply box filter" << std::endl;
    BoxFilter<float> boxFilter(5, 3);
    boxFilter(img, oimg);

    cve::imsave("box_smooth." + ext, oimg);

    std::cout << "Apply gauss filter" << std::endl;
    GaussFilter<float> gaussFilter(3);
    gaussFilter(img, oimg);

    cve::imsave("gauss_smooth." + ext, oimg);


    std::cout << "Apply recursive filter" << std::endl;
    RecursiveBlurFilter<float> recFilter(3);
    recFilter(img, oimg);

    cve::imsave("recursive_smooth." + ext, oimg);

    return 0;
}
