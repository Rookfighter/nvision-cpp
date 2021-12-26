#include <iostream>
#include <cve/filter.h>
#include <cve/imageio.h>

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_canny <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Each value per pixel and depth is representd by
    // a 8-Bit integer.
    cve::Image<cve::Grayf> src;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    std::cout << "Load " << argv[1] << std::endl;
    cve::imload(argv[1], src);

    // Create a Canny filter object. The template parameter determines the
    // internal Scalar type, which is used for computations (e.g. Kernel and
    // Kernel application).
    cve::CannyEdgeFilter<cve::float32> filter;

    // Apply the filter to the source image and store it in dest.
    std::cout << "Apply filter" << std::endl;
    cve::Image<cve::Grayf> dest = filter(src);

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    std::cout << "Save " << argv[2] << std::endl;
    cve::imsave(argv[2], dest);

    return 0;
}
