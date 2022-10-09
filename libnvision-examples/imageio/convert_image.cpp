
#include <nvision/imageio.h>
#include <iostream>

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: convert_img <src file> <dst file>" << std::endl;
        return 1;
    }

    std::string srcFile = argv[1];
    std::string dstFile = argv[2];

    std::cout << "Read " << srcFile << std::endl;
    nvision::Image<nvision::RGBA> image;
    nvision::imload(srcFile, image);

    std::cout << "Write " << dstFile << std::endl;
    nvision::imsave(dstFile, image);

    return 0;
}
