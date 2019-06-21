/* detect_corners.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <cve/feature/fast_detector.h>
#include <cve/feature/brief_descriptor.h>
#include <cve/feature/orb_descriptor.h>
#include <cve/draw/shape_drawer.h>
#include <cve/draw/colors.h>
#include <cve/imageio/imageio.h>

using namespace cve;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> Matrix;
typedef Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu8;

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: find_matches <filename> <filename>" << std::endl;
        return -1;
    }

    std::cout << "Load \"" << argv[1] << "\" and \"" << argv[2] << "\"" << std::endl;
    Image8 imgA;
    Image8 imgB;
    Image8 grayA;
    Image8 grayB;
    Image8 oimg;
    Matrix keypointsA;
    Matrix keypointsB;
    Matrixu8 descriptorsA;
    Matrixu8 descriptorsB;
    std::string ext = cve::extension(argv[1]);

    cve::imload(argv[1], imgA);
    cve::imload(argv[2], imgB);

    cve::image::rgb2gray(imgA, grayA);
    cve::image::rgb2gray(imgB, grayB);

    std::cout << "Apply FAST detector" << std::endl;
    FASTDetector<float> fastDetector;
    fastDetector.detect(grayA, keypointsA);
    fastDetector.detect(grayB, keypointsB);

    std::cout << "kpA:" << keypointsA.cols() << " kpB:" << keypointsB.cols() << std::endl;

    std::cout << "Compute BRIEF descriptors" << std::endl;
    BRIEFDescriptor<float> briefDescriptor;
    briefDescriptor.compute(grayA, keypointsA, descriptorsA);
    briefDescriptor.compute(grayB, keypointsB, descriptorsB);

    std::cout << std::hex << descriptorsA.block(0,0,16,5) << std::endl;

    std::cout << "Compute ORB descriptors" << std::endl;
    ORBDescriptor<float> orbDescriptor;
    orbDescriptor.compute(grayA, keypointsA, descriptorsA);
    orbDescriptor.compute(grayB, keypointsB, descriptorsB);

    // cve::imsave("fast_corners." + ext, oimg);

    return 0;
}
