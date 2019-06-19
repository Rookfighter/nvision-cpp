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
#include <cve/imageio/imageio.h>

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
    cve::imload(argv[1], img);
    std::string ext = cve::extension(argv[1]);

    std::cout << "Apply sobel filter" << std::endl;
    SobelFilter<float> sobelFilter;
    sobelFilter.apply(img, oimg);
    image::normalize(oimg, 0.0f, 255.0f);

    cve::imsave("sobel_edges." + ext, oimg);

    std::cout << "Apply scharr filter" << std::endl;
    ScharrFilter<float> scharrFilter;
    scharrFilter.apply(img, oimg);
    image::normalize(oimg, 0.0f, 255.0f);

    cve::imsave("scharr_edges." + ext, oimg);

    std::cout << "Apply laplace filter" << std::endl;
    LaplaceFilter<float> laplaceFilter;
    laplaceFilter.apply(img, oimg);
    image::normalize(oimg, 0.0f, 255.0f);

    cve::imsave("laplace_edges." + ext, oimg);

    std::cout << "Apply canny detector" << std::endl;
    CannyDetector<float> cannyDetector;
    cannyDetector.setSmoothFilter({1.4});
    cannyDetector.apply(img, oimg);

    cve::imsave("canny_edges." + ext, oimg);

    return 0;
}
