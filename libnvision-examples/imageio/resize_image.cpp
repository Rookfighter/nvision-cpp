
#include <nvision/imageio.h>
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
    nvision::float32 factor = std::stof(argv[3]);

    std::cout << "Read " << srcFile << std::endl;
    nvision::Image<nvision::RGB> image;
    nvision::imload(srcFile, image);

    std::cout << "Resize by " << factor << std::endl;
    nvision::Image<nvision::RGB> dstImg = nvision::image::resize(image, factor);

    std::cout << "Write " << dstFile << std::endl;
    nvision::imsave(dstFile, dstImg);

    return 0;
}
