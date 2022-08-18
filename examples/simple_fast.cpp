#include <iostream>
#include <nvision/feature/fast_detector.h>
#include <nvision/draw/shape_drawer.h>
#include <nvision/draw/colors.h>
#include <nvision/imageio/imageio.h>

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_fast <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Declare Image tensors. Each value per pixel and depth is representd by
    // a float.
    // There are also Image8, Image16, Image32 or Imaged available for
    // 8-bit, 16-bit, 32-bit and double types respectively-
    nvision::Imagef src;
    nvision::Imagef dest;

    nvision::Matrixf keypoints;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    // Currently PPM PGM, PNG and JPG are supported.
    nvision::imload(argv[1], src);

    // Create a fast feature object. The template parameter determines the
    // internal Scalar type, which is used for computations.
    // This constructor expects the standard deviation of the gaussian function
    // and calculates the kernel size automatically.
    nvision::FASTDetector<float> fast;

    // Apply the FAST detector to the source image and stores its keypoints.
    fast(src, keypoints);

    // Create an output image with the detected keypoints.
    // Copy input image to output image.
    dest = src;
    // Draw circles of radius of 10 around the keypoints
    nvision::ShapeDrawer<float> drawer;
    drawer.drawCircle(keypoints, 10, nvision::Colorf::Red(), dest);

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    // Currently PPM PGM, PNG and JPG are supported.
    nvision::imsave(argv[2], dest);
}
