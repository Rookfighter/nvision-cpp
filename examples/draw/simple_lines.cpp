/* simple_lines.cpp
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
        std::cerr << "usage: simple_lines <out-file>" << std::endl;
        return -1;
    }

    constexpr auto width = cve::Index{400};
    constexpr auto height = cve::Index{600};
    constexpr auto widthf = static_cast<cve::float32>(width);
    constexpr auto heightf = static_cast<cve::float32>(height);
    constexpr auto offsetX = widthf * 0.1f;
    constexpr auto offsetY = heightf * 0.1f;

    Image<RGBf> img(height, width);
    img.setOnes();

    cve::image::draw(img, cve::Line(offsetX, offsetY, widthf - offsetX, heightf - offsetY), cve::color::black<RGBf>());
    cve::image::draw(img, cve::Line(offsetX, heightf - offsetY, widthf - offsetX, offsetY), cve::color::red<RGBf>());
    cve::image::draw(img, cve::Line(offsetX, heightf * 0.5f, widthf - offsetX, heightf * 0.5f), cve::color::magenta<RGBf>());
    cve::image::draw(img, cve::Line(widthf * 0.5f, offsetY, widthf * 0.5f, heightf - offsetY), cve::color::blue<RGBf>());

    std::cout << "Save " << argv[1] << std::endl;
    cve::imsave(argv[1], img);

    return 0;
}
