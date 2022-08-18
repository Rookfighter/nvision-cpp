/* detect_corners.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <nvision/feature/harris_detector.h>
#include <nvision/feature/shi_tomasi_detector.h>
#include <nvision/feature/fast_detector.h>
#include <nvision/feature/orb_detector.h>
#include <nvision/draw/shape_drawer.h>
#include <nvision/draw/colors.h>
#include <nvision/imageio/imageio.h>

using namespace nvision;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> Matrix;

int main(int argc, const char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: detect_corners <filename>" << std::endl;
        return -1;
    }

    std::cout << "Load \"" << argv[1] << "\"" << std::endl;
    Imagef img;
    Imagef oimg;
    Matrix keypoints;
    nvision::imload(argv[1], img);
    std::string ext = nvision::extension(argv[1]);

    Imagef imgGray;
    nvision::image::rgb2gray(img, imgGray);

    std::cout << "Apply harris detector" << std::endl;
    HarrisDetector<float> harris;
    harris(img, keypoints);

    oimg = img;
    ShapeDrawer<float> shapeDrawer;
    shapeDrawer.drawCircle(keypoints, 10, Colorf::Red(), oimg);

    nvision::imsave("harris_corners." + ext, oimg);

    std::cout << "Apply shi tomasi detector" << std::endl;
    ShiTomasiDetector<float> shi;
    shi(img, keypoints);

    oimg = img;
    shapeDrawer.drawCircle(keypoints, 10, Colorf::Red(), oimg);

    nvision::imsave("shi_tomasi_corners." + ext, oimg);

    std::cout << "Apply FAST detector" << std::endl;
    FASTDetector<float> fast;
    fast(imgGray, keypoints);

    oimg = img;
    shapeDrawer.drawCircle(keypoints, 10, Colorf::Red(), oimg);

    nvision::imsave("fast_corners." + ext, oimg);

    std::cout << "Apply ORB detector" << std::endl;
    ORBDetector<float> orb;
    orb(imgGray, keypoints);

    oimg = img;
    shapeDrawer.drawCircle(keypoints, 10, Colorf::Red(), oimg);

    nvision::imsave("orb_corners." + ext, oimg);

    return 0;
}
