/* filter_smooth.cpp
 *
 * Author: Fabian Meyer
 * Created On: 02 Mar 2019
 */

#include <iostream>
#include <cve/filter/sobel_filter.h>
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

    for(Index c = 0; c < oimg.cols(); ++c)
    {
        for(Index r = 0; r < oimg.rows(); ++r)
        {
            for(Index d = 0; d < oimg(r, c).size(); ++d)
            {
                float val = oimg(r, c)(d);
                img(r, c)(d) = std::min(255.0f, std::max(0.0f, val));
            }
        }
    }

    cve::pgm::save("sobel_edges.pgm", img);

    return 0;
}
