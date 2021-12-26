/* image_type.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_IMAGE_TYPE_H_
#define CVE_IMAGE_TYPE_H_

#include <Eigen/Core>
#include "cve/src/core/pixel.h"

#define CVE_GEN_SCALAR_OPERATORS(Scalar)\
    template<typename ColorSpace>\
    inline decltype(auto) operator+(const Image<ColorSpace> &lhs, const Scalar rhs)\
    {\
        return lhs.unaryExpr([rhs](const Pixel<ColorSpace> &pixel) { return pixel + rhs; });\
    }\
    template<typename ColorSpace>\
    inline decltype(auto) operator+(const Scalar lhs, const Image<ColorSpace> &rhs)\
    {\
        return rhs.unaryExpr([lhs](const Pixel<ColorSpace> &pixel) { return lhs + pixel; });\
    }\
    template<typename ColorSpace>\
    inline decltype(auto) operator-(const Image<ColorSpace> &lhs, const Scalar rhs)\
    {\
        return lhs.unaryExpr([rhs](const Pixel<ColorSpace> &pixel) { return pixel - rhs; });\
    }\
    template<typename ColorSpace>\
    inline decltype(auto) operator-(const Scalar lhs, const Image<ColorSpace> &rhs)\
    {\
        return rhs.unaryExpr([lhs](const Pixel<ColorSpace> &pixel) { return lhs - pixel; });\
    }\
    template<typename ColorSpace>\
    inline decltype(auto) operator*(const Image<ColorSpace> &lhs, const Scalar rhs)\
    {\
        return lhs.unaryExpr([rhs](const Pixel<ColorSpace> &pixel) { return pixel * rhs; });\
    }\
    template<typename ColorSpace>\
    inline decltype(auto) operator*(const Scalar lhs, const Image<ColorSpace> &rhs)\
    {\
        return rhs.unaryExpr([lhs](const Pixel<ColorSpace> &pixel) { return lhs * pixel; });\
    }\
    template<typename ColorSpace>\
    inline decltype(auto) operator/(const Image<ColorSpace> &lhs, const Scalar rhs)\
    {\
        return lhs.unaryExpr([rhs](const Pixel<ColorSpace> &pixel) { return pixel / rhs; });\
    }\
    template<typename ColorSpace>\
    inline decltype(auto) operator/(const Scalar lhs, const Image<ColorSpace> &rhs)\
    {\
        return rhs.unaryExpr([lhs](const Pixel<ColorSpace> &pixel) { return lhs / pixel; });\
    }

namespace cve
{
    template<typename ColorSpace>
    using Image = Eigen::Array<Pixel<ColorSpace>, Eigen::Dynamic, Eigen::Dynamic>;

    using ImageGray = Image<Gray>;
    using ImageRGB = Image<RGB>;
    using ImageBGR = Image<BGR>;
    using ImageRGBA = Image<RGBA>;
    using ImageBGRA = Image<BGRA>;
    using ImageHSV = Image<HSV>;

    CVE_GEN_SCALAR_OPERATORS(int8)
    CVE_GEN_SCALAR_OPERATORS(int16)
    CVE_GEN_SCALAR_OPERATORS(int32)
    CVE_GEN_SCALAR_OPERATORS(int64)

    CVE_GEN_SCALAR_OPERATORS(uint8)
    CVE_GEN_SCALAR_OPERATORS(uint16)
    CVE_GEN_SCALAR_OPERATORS(uint32)
    CVE_GEN_SCALAR_OPERATORS(uint64)

    CVE_GEN_SCALAR_OPERATORS(float32)
    CVE_GEN_SCALAR_OPERATORS(float64)
}

#endif
