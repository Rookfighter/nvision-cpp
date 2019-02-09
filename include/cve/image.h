/* image.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_IMAGE_H_
#define CVE_IMAGE_H_

namespace cve
{
    typedef Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> GrayImage;
    typedef Eigen::Matrix<uint8_t, 3, 1> RGBPixel;
    typedef Eigen::Matrix<RGBPixel, Eigen::Dynamic, Eigen::Dynamic> RGBImage;
}

#endif
