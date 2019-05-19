/* filter_smooth.cpp
 *
 * Author: Fabian Meyer
 * Created On: 02 Mar 2019
 */

#include <iostream>
#include <cve/filter/gauss_filter.h>
#include <cve/filter/box_filter.h>
#include <cve/filter/recursive_blur_filter.h>
#include <cve/imageio/pgm.h>

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
    cve::pgm::load(argv[1], img);

    std::cout << "Apply box filter" << std::endl;
    BoxFilter<float, 5> boxFilter(3);
    boxFilter.apply<uint8_t, uint8_t>(img, oimg);

    cve::pgm::save("box_smooth.pgm", oimg);

    std::cout << "Apply gauss filter" << std::endl;
    GaussFilter<float, 9> gaussFilter(3);
    gaussFilter.apply(img, oimg);

    cve::pgm::save("gauss_smooth.pgm", oimg);


    std::cout << "Apply recursive filter" << std::endl;
    RecursiveBlurFilter<float> recFilter(3);
    recFilter.apply(img, oimg);

    cve::pgm::save("recursive_smooth.pgm", oimg);

    return 0;
}
