/* filter_smooth.cpp
 *
 * Author: Fabian Meyer
 * Created On: 02 Mar 2019
 */

#include <iostream>
#include <cve/filter/gauss_filter.h>
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
    ImageGray img;
    cve::pgm::load(argv[1], img);

    std::cout << "Apply gauss filter" << std::endl;
    GaussFilter<float> gaussFilter(20);
    ImageGray oimg;
    gaussFilter.apply<ImageGray>(img, oimg);

    cve::pgm::save("gauss_smooth.pgm", oimg);

    return 0;
}
