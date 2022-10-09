/* simple_lucas_kanade.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <nvision/optflow.h>
#include <nvision/imageio.h>

using namespace nvision;

int main(int argc, const char **argv)
{
    if(argc != 4)
    {
        std::cerr << "usage: simple_lucas_kanade <in-file-a> <in-file-b> <out-file>" << std::endl;
        return -1;
    }

    // Each value per pixel and depth is representd by a float
    nvision::Image<nvision::RGBf> srcA;
    nvision::Image<nvision::RGBf> srcB;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    std::cout << "Load " << argv[1] << std::endl;
    nvision::imload(argv[1], srcA);
    std::cout << "Load " << argv[2] << std::endl;
    nvision::imload(argv[2], srcB);

    // Create a Lucas Kanade flow detector object. The template parameter determines the
    // internal Scalar type, which is used for computations and flow vector representation.
    nvision::LucasKanadeFlow<nvision::float32> detector;

    nvision::FlowField<nvision::float32> flow;
    std::cout << "Compute flow" << std::endl;
    detector(srcA, srcB, flow);

    std::cout << "Create colored flow" << std::endl;
    Image<RGBf> flowImg = nvision::imflow<RGBf>(flow);

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    std::cout << "Save " << argv[3] << std::endl;
    nvision::imsave(argv[3], flowImg);

    return 0;
}
