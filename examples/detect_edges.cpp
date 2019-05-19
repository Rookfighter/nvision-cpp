/* filter_smooth.cpp
 *
 * Author: Fabian Meyer
 * Created On: 02 Mar 2019
 */

#include <iostream>
#include <cve/filter/sobel_filter.h>
#include <cve/filter/scharr_filter.h>
#include <cve/filter/laplace_filter.h>
#include <cve/feature/canny_detector.h>
#include <cve/imageio/pgm.h>

using namespace cve;

int main(int argc, const char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: detect_edges <filename>" << std::endl;
        return -1;
    }

    std::cout << "Load \"" << argv[1] << "\"" << std::endl;
    Imagef img;
    Imagef oimg;
    cve::pgm::load(argv[1], img);

    Eigen::Tensor<float, 1> pixLow(1);
    pixLow.setConstant(0);
    Eigen::Tensor<float, 1> pixHigh(1);
    pixHigh.setConstant(255);

    std::cout << "Apply sobel filter" << std::endl;
    SobelFilter<float> sobelFilter;
    sobelFilter.apply(img, oimg);
    image::normalize(oimg, pixLow, pixHigh);

    cve::pgm::save("sobel_edges.pgm", oimg);

    std::cout << "Apply scharr filter" << std::endl;
    ScharrFilter<float> scharrFilter;
    scharrFilter.apply(img, oimg);
    image::normalize(oimg, pixLow, pixHigh);

    cve::pgm::save("scharr_edges.pgm", oimg);

    std::cout << "Apply laplace filter" << std::endl;
    LaplaceFilter<float> laplaceFilter;
    laplaceFilter.apply(img, oimg);
    image::normalize(oimg, pixLow, pixHigh);

    cve::pgm::save("laplace_edges.pgm", oimg);

    std::cout << "Apply canny detector" << std::endl;
    CannyDetector<float> cannyDetector;
    cannyDetector.setSmoothFilter({1.4});
    cannyDetector.apply(img, oimg);

    cve::pgm::save("canny_edges.pgm", oimg);

    return 0;
}
