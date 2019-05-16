/* filter_smooth.cpp
 *
 * Author: Fabian Meyer
 * Created On: 02 Mar 2019
 */

#include <iostream>
#include <cve/filter/sobel_filter.h>
#include <cve/filter/scharr_filter.h>
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
    ImageGray img;
    cve::pgm::load(argv[1], img);

    std::cout << "Apply sobel filter" << std::endl;
    SobelFilter<float> sobelFilter;
    ImageGrayf oimg;
    sobelFilter.apply(img, oimg);

    ImageGray oimg2;
    image::normalize(oimg, ImageGrayf::Pixel(0), ImageGrayf::Pixel(255));
    image::copy(oimg, oimg2);

    cve::pgm::save("sobel_edges.pgm", oimg2);

    std::cout << "Apply scharr filter" << std::endl;
    ScharrFilter<float> scharrFilter;
    scharrFilter.apply(img, oimg);

    image::normalize(oimg, ImageGrayf::Pixel(0), ImageGrayf::Pixel(255));
    image::copy(oimg, oimg2);

    cve::pgm::save("scharr_edges.pgm", oimg2);

    std::cout << "Apply canny detector" << std::endl;
    CannyDetector<float> cannyDetector;
    cannyDetector.setSmoothFilter(GaussFilter<float, 5>(1.4));
    cannyDetector.apply(img, oimg);

    image::copy(oimg, oimg2);

    cve::pgm::save("canny_edges.pgm", oimg2);

    return 0;
}
