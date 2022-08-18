/* resize_image.cpp
 *
 * Author: Fabian Meyer
 * Created On: 29 Apr 2019
 */

#include <iostream>
#include <nvision/core/image.h>
#include <nvision/imageio/pgm.h>

int main(int argc, const char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: resize_image <filename>" << std::endl;
        return -1;
    }

    std::cout << "Load \"" << argv[1] << "\"" << std::endl;
    nvision::Image8 img;
    nvision::pgm::load(argv[1], img);

    std::cout << img.dimension(0) << ',' << img.dimension(1) << ',' << img.dimension(2) << std::endl;

    double factor = 0.3;
    std::cout << "Resize " << factor << std::endl;;
    nvision::Image8 oimg;
    nvision::image::resize(factor, img, oimg);

    nvision::pgm::save("resized_image.pgm", oimg);

    return 0;
}
