/* optical_flow.cpp
 *
 * Author: Fabian Meyer
 * Created On: 16 Mar 2019
 */

#include <iostream>
#include <cve/optflow/lucas_kanade_detector.h>
#include <cve/optflow/color_map.h>
#include <cve/imageio/pgm.h>
#include <cve/imageio/ppm.h>

using namespace cve;

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: optical_flow <filename> <filename>" << std::endl;
        return -1;
    }

    std::cout << "Load \"" << argv[1] << "\"" << std::endl;
    ImageGray imgA;
    cve::pgm::load(argv[1], imgA);
    std::cout << "-- size " << imgA.cols() << "x" << imgA.rows() << std::endl;

    std::cout << "Load \"" << argv[2] << "\"" << std::endl;
    ImageGray imgB;
    cve::pgm::load(argv[2], imgB);
    std::cout << "-- size " << imgB.cols() << "x" << imgB.rows() << std::endl;

    GaussFilter<float, 7> preSmooth(1);
    preSmooth.apply(imgA);
    preSmooth.apply(imgB);

    std::cout << "Apply lucas kanade detector" << std::endl;

    typename LucasKanadeDetector<float>::FlowImage flowImg;
    LucasKanadeDetector<float, GaussFilter<float, 7>> lkDetector;
    lkDetector.setSmoothFilter({1});
    lkDetector.apply(imgA, imgB, flowImg);
    ImageRGB oimg;
    optflow::ColorMap<float> cmap;
    cmap.apply(flowImg, oimg);

    cve::ppm::save("lucas_kanade.ppm", oimg);

    return 0;
}
