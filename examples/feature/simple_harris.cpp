/* simple_harris.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <nvision/feature.h>
#include <nvision/draw.h>
#include <nvision/imageio.h>

using namespace nvision;

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_harris <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Each value per pixel and depth is representd by
    // a 8-Bit integer.
    nvision::Image<nvision::RGBf> src;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    std::cout << "Load " << argv[1] << std::endl;
    nvision::imload(argv[1], src);

    std::cout << "Convert to gray scale" << std::endl;
    nvision::Image<nvision::Grayf> gray = nvision::image::convert<Grayf>(src);

    // Create a Harris feature detector object. The template parameter determines the
    // internal Scalar type, which is used for computations and feature point
    // representation.
    nvision::HarrisFeature<nvision::float32> detector;

    // Extract feature points from the image.
    using FeatureMatrix = typename nvision::HarrisFeature<nvision::float32>::FeatureMatrix;
    FeatureMatrix keypoints;
    std::cout << "Detect features" << std::endl;
    detector(gray, keypoints);

    std::cout << "Draw " << keypoints.cols() << " keypoints" << std::endl;
    nvision::image::draw(src, keypoints, nvision::Marker::Circle, nvision::color::red<RGBf>());

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    std::cout << "Save " << argv[2] << std::endl;
    nvision::imsave(argv[2], src);

    return 0;

    return 0;
}
