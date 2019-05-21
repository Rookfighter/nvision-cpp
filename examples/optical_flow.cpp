/* optical_flow.cpp
 *
 * Author: Fabian Meyer
 * Created On: 16 Mar 2019
 */

#include <iostream>
#include <cve/optflow/lucas_kanade_detector.h>
#include <cve/optflow/horn_schunck_detector.h>
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
    Image8 imgA;
    cve::pgm::load(argv[1], imgA);
    std::cout << "-- size " << imgA.dimension(1) << "x" << imgA.dimension(0) << std::endl;

    std::cout << "Load \"" << argv[2] << "\"" << std::endl;
    Image8 imgB;
    cve::pgm::load(argv[2], imgB);
    std::cout << "-- size " << imgB.dimension(1) << "x" << imgB.dimension(0) << std::endl;

    GaussFilter<float, 9> preSmooth(2);
    preSmooth.apply(imgA);
    preSmooth.apply(imgB);

    std::cout << "Apply Lucas Kanade detector" << std::endl;

    Eigen::Tensor<float, 3> flowImg;
    LucasKanadeDetector<float, GaussFilter<float, 9>> lkDetector;
    lkDetector.setSmoothFilter({1.4});
    lkDetector.apply(imgA, imgB, flowImg);
    Image8 oimg;
    optflow::ColorMap<float> cmap;
    cmap.apply(flowImg, oimg);

    cve::ppm::save("lucas_kanade.ppm", oimg);

    std::cout << "Apply Horn Schunck detector" << std::endl;

    HornSchunckDetector<float> hsDetector;
    hsDetector.apply(imgA, imgB, flowImg);
    cmap.apply(flowImg, oimg);

    cve::ppm::save("horn_schunck.ppm", oimg);

    return 0;
}
