/* simple_lines.cpp
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
        std::cerr << "usage: simple_lines <out-file>" << std::endl;
        return -1;
    }

    constexpr auto width = nvision::Index{400};
    constexpr auto height = nvision::Index{600};
    constexpr auto widthf = static_cast<nvision::float32>(width);
    constexpr auto heightf = static_cast<nvision::float32>(height);
    constexpr auto offsetX = widthf * 0.1f;
    constexpr auto offsetY = heightf * 0.1f;

    Image<RGBf> img(height, width);
    img.setOnes();

    nvision::image::draw(img, nvision::Line(offsetX, offsetY, widthf - offsetX, heightf - offsetY), nvision::color::black<RGBf>());
    nvision::image::draw(img, nvision::Line(offsetX, heightf - offsetY, widthf - offsetX, offsetY), nvision::color::red<RGBf>());
    nvision::image::draw(img, nvision::Line(offsetX, heightf * 0.5f, widthf - offsetX, heightf * 0.5f), nvision::color::magenta<RGBf>());
    nvision::image::draw(img, nvision::Line(widthf * 0.5f, offsetY, widthf * 0.5f, heightf - offsetY), nvision::color::blue<RGBf>());

    std::cout << "Save " << argv[1] << std::endl;
    nvision::imsave(argv[1], img);

    return 0;
}
