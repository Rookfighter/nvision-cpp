/* colorwheel.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <cve/optflow.h>
#include <cve/imageio.h>

using namespace cve;

int main(int argc, const char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: color_map <out-file>" << std::endl;
        return -1;
    }

    constexpr auto dim = cve::Index{400};
    constexpr auto radius = 0.5f * static_cast<cve::float32>(dim);
    cve::FlowField<cve::float32> flow;
    flow.resize(dim, dim);
    for(Index c = 0; c < flow.cols(); ++c)
    {
        for(Index r = 0; r < flow.rows(); ++r)
        {
            const auto dx = static_cast<cve::float32>(c) - radius;
            const auto dy = static_cast<cve::float32>(r) - radius;
            const auto inside = static_cast<cve::float32>(dx * dx + dy * dy) <= radius * radius;
            if(inside)
                flow(r, c) << dx, dy;
            else
                flow(r, c) << 0, 0;
        }
    }

    std::cout << "Create colored flow" << std::endl;
    Image<RGBf> flowImg = cve::imflow<RGBf>(flow);

    std::cout << "Save " << argv[1] << std::endl;
    cve::imsave(argv[1], flowImg);

    return 0;
}
