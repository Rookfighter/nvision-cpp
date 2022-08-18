/* filled_polygon.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <nvision/draw.h>
#include <nvision/imageio.h>

using namespace nvision;

int main(int argc, const char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: filled_polygon <out-file>" << std::endl;
        return -1;
    }

    constexpr auto width = nvision::Index{400};
    constexpr auto height = nvision::Index{600};

    nvision::Polygon polygon;
    polygon.fillMode = nvision::FillMode::Fill;

    polygon.points.push_back({10, 300});
    polygon.points.push_back({150, 50});
    polygon.points.push_back({250, 80});
    polygon.points.push_back({350, 300});
    polygon.points.push_back({270, 500});
    polygon.points.push_back({40, 410});

    Image<RGBf> img(height, width);
    img.setOnes();
    nvision::image::draw(img, polygon, nvision::color::black<nvision::RGBf>());

    std::cout << "Save " << argv[1] << std::endl;
    nvision::imsave(argv[1], img);

    return 0;
}
