/* upsample.cpp
 *
 * Author: Fabian Meyer
 * Created On: 29 Apr 2019
 */

#include <iostream>
#include <cve/core/image.h>
#include <cve/imageio/pgm.h>

int main(int argc, const char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: upsample <filename>" << std::endl;
        return -1;
    }

    std::cout << "Load \"" << argv[1] << "\"" << std::endl;
    cve::Image8 img;
    cve::pgm::load(argv[1], img);

    std::cout << img.dimension(0) << ',' << img.dimension(1) << ',' << img.dimension(2) << std::endl;

    double factor = 1.3;
    std::cout << "Upsample " << factor << std::endl;;
    cve::Image8 oimg;
    cve::image::upsample(factor, img, oimg);

    cve::pgm::save("upsample.pgm", oimg);

    return 0;
}
