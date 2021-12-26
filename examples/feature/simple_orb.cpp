/* simple_orb.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <cve/feature.h>
#include <cve/draw.h>
#include <cve/imageio.h>

using namespace cve;

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_orb <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Each value per pixel and depth is representd by
    // a 8-Bit integer.
    cve::Image<cve::RGBf> src;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    std::cout << "Load " << argv[1] << std::endl;
    cve::imload(argv[1], src);

    std::cout << "Convert to gray scale" << std::endl;
    cve::Image<cve::Grayf> gray = cve::image::convert<RGBf, Grayf>(src);

    // Create a ORB feature detector object. The template parameter determines the
    // internal Scalar type, which is used for computations and feature point
    // representation.
    cve::ORBFeature<cve::float32> detector;

    // Extract feature points from the image.
    using FeatureMatrix = typename cve::ORBFeature<cve::float32>::FeatureMatrix;
    FeatureMatrix keypoints;
    std::cout << "Detect features" << std::endl;
    detector(gray, keypoints);

    std::cout << "Draw " << keypoints.cols() << " keypoints" << std::endl;
    cve::image::draw(src, keypoints, cve::Marker::Circle, cve::color::red<RGBf>());

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    std::cout << "Save " << argv[2] << std::endl;
    cve::imsave(argv[2], src);

    return 0;

    return 0;
}
