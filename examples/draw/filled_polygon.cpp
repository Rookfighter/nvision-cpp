/* filled_polygon.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <cve/draw.h>
#include <cve/imageio.h>

using namespace cve;

int main(int argc, const char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: filled_polygon <out-file>" << std::endl;
        return -1;
    }

    constexpr auto width = cve::Index{400};
    constexpr auto height = cve::Index{600};

    cve::Polygon polygon;
    polygon.fillMode = cve::FillMode::Fill;

    polygon.points.push_back({10, 300});
    polygon.points.push_back({150, 50});
    polygon.points.push_back({250, 80});
    polygon.points.push_back({350, 300});
    polygon.points.push_back({270, 500});
    polygon.points.push_back({40, 410});

    Image<RGBf> img(height, width);
    img.setOnes();
    cve::image::draw(img, polygon, cve::color::black<cve::RGBf>());

    std::cout << "Save " << argv[1] << std::endl;
    cve::imsave(argv[1], img);

    return 0;
}
