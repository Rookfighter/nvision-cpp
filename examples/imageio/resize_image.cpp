
#include <cve/imageio.h>
#include <iostream>

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        std::cerr << "usage: convert_img <src file> <dst file> <factor>" << std::endl;
        return 1;
    }

    std::string srcFile = argv[1];
    std::string dstFile = argv[2];
    cve::float32 factor = std::stof(argv[3]);

    std::cout << "Read " << srcFile << std::endl;
    cve::Image<cve::RGB> image;
    cve::imload(srcFile, image);

    std::cout << "Resize by " << factor << std::endl;
    cve::Image<cve::RGB> dstImg = cve::image::resize(image, factor);

    std::cout << "Write " << dstFile << std::endl;
    cve::imsave(dstFile, dstImg);

    return 0;
}