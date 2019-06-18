/* detect_corners.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <cve/feature/harris_detector.h>
#include <cve/imageio/pgm.h>

using namespace cve;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> Matrix;

int main(int argc, const char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: detect_corners <filename>" << std::endl;
        return -1;
    }

    std::cout << "Load \"" << argv[1] << "\"" << std::endl;
    Imagef img;
    Imagef oimg;
    Matrix keypoints;
    cve::pgm::load(argv[1], img);

    std::cout << "Apply harris detector" << std::endl;
    HarrisDetector<float> harrisDetector;
    harrisDetector.apply(img, keypoints);

    cve::pgm::save("harris_corners.pgm", oimg);

    return 0;
}
