#include <iostream>
#include <nvision/filter.h>
#include <nvision/imageio.h>

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_canny <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Each value per pixel and depth is representd by
    // a 8-Bit integer.
    nvision::Image<nvision::Grayf> src;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    std::cout << "Load " << argv[1] << std::endl;
    nvision::imload(argv[1], src);

    nvision::GaussFilter<nvision::float32, 3> smoothFilter;
    nvision::SobelFilter<nvision::float32> gradientFilter;

    // Create a Canny filter object. The template parameter determines the
    // internal Scalar type, which is used for computations (e.g. Kernel and
    // Kernel application).
    nvision::CannyEdgeFilter<nvision::float32> edgeFilter;

    // Apply the filter to the source image and store it in dest.
    std::cout << "Apply filter" << std::endl;
    nvision::Image<nvision::Grayf> smooth = smoothFilter(src);
    nvision::Image<nvision::Grayf> gradientX = gradientFilter(smooth, nvision::GradientMode::X{});
    nvision::Image<nvision::Grayf> gradientY = gradientFilter(smooth, nvision::GradientMode::Y{});
    nvision::Image<nvision::Grayf> dest = edgeFilter(gradientX, gradientY);

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    std::cout << "Save " << argv[2] << std::endl;
    nvision::imsave(argv[2], dest);

    return 0;
}
