#include <iostream>
#include <nvision/filter.h>
#include <nvision/imageio.h>

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_box <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Each value per pixel and depth is representd by
    // a 8-Bit integer.
    nvision::Image<nvision::RGB> src;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    std::cout << "Load " << argv[1] << std::endl;
    nvision::imload(argv[1], src);

    // Create a Box filter object. The template parameter determines the
    // internal Scalar type, which is used for computations (e.g. Kernel and
    // Kernel application).
    nvision::BoxFilter<nvision::float32, 9> filter;

    // Apply the filter to the source image and store it in dest.
    std::cout << "Apply filter" << std::endl;
    nvision::Image<nvision::RGB> dest = filter(src);

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    std::cout << "Save " << argv[2] << std::endl;
    nvision::imsave(argv[2], dest);

    return 0;
}
