#include <iostream>
#include <cve/filter/gauss_filter.h>
#include <cve/imageio/imageio.h>
#include <cve/core/image.h>

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_gauss <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Declare Image tensors. Each value per pixel and depth is representd by
    // a 8-Bit integer.
    // There are also Image16, Image32, Imagef or Imaged available for
    // 16-bit, 32-bit, float and double types respectively-
    cve::Image8 src;
    cve::Image8 dest;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    // Currently PPM PGM, PNG and JPG are supported.
    cve::imload(argv[1], src);

    // Create a Gauss filter object. The template parameter determines the
    // internal Scalar type, which is used for computations (e.g. Kernel and
    // Kernel application).
    // This constructor expects the standard deviation of the gaussian function
    // and calculates the kernel size automatically.
    cve::GaussFilter<float> gaussFilter(3);

    // Apply the Gauss filter to the source image and store it in dest.
    gaussFilter(src, dest);

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    // Currently PPM PGM, PNG and JPG are supported.
    cve::imsave(argv[2], dest);
}
