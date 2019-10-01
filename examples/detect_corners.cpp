/* detect_corners.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <cve/feature/harris_features.h>
#include <cve/feature/shi_tomasi_features.h>
#include <cve/feature/fast_features.h>
#include <cve/draw/shape_drawer.h>
#include <cve/draw/colors.h>
#include <cve/imageio/imageio.h>

using namespace cve;

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
    cve::imload(argv[1], img);
    std::string ext = cve::extension(argv[1]);

    Imagef imgGray;
    cve::image::rgb2gray(img, imgGray);

    std::cout << "Apply harris detector" << std::endl;
    HarrisFeatures<float> harris;
    harris(img, keypoints);

    oimg = img;
    ShapeDrawer<float> shapeDrawer;
    shapeDrawer.drawCircle(keypoints, 10, Colorf::Red(), oimg);

    cve::imsave("harris_corners." + ext, oimg);

    std::cout << "Apply shi tomasi detector" << std::endl;
    ShiTomasiFeatures<float> shi;
    shi(img, keypoints);

    oimg = img;
    shapeDrawer.drawCircle(keypoints, 10, Colorf::Red(), oimg);

    cve::imsave("shi_tomasi_corners." + ext, oimg);

    std::cout << "Apply FAST detector" << std::endl;
    FASTFeatures<float> fast;
    fast(imgGray, keypoints);

    oimg = img;
    shapeDrawer.drawCircle(keypoints, 10, Colorf::Red(), oimg);

    cve::imsave("fast_corners." + ext, oimg);

    return 0;
}
