/* image_type.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef NVISION_IMAGE_TYPE_H_
#define NVISION_IMAGE_TYPE_H_

#include <Eigen/Core>
#include "nvision/src/core/pixel.h"

#define NVISION_GEN_SCALAR_OPERATORS(Scalar)\
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

namespace nvision
{
    template<typename T>
    struct IsImage
    {
        static constexpr bool value = false;
    };

    template<typename ColorSpace>
    using Image = Eigen::Array<Pixel<ColorSpace>, Eigen::Dynamic, Eigen::Dynamic>;

    template<typename Derived>
    using ImageBase = Eigen::ArrayBase<Derived>;

    template<typename Derived>
    struct IsImage<ImageBase<Derived>>
    {
        static constexpr bool value = IsPixel<typename ImageBase<Derived>::Scalar>::value;
    };

    using ImageGray = Image<Gray>;
    using ImageRGB = Image<RGB>;
    using ImageBGR = Image<BGR>;
    using ImageRGBA = Image<RGBA>;
    using ImageBGRA = Image<BGRA>;
    using ImageHSV = Image<HSV>;

    NVISION_GEN_SCALAR_OPERATORS(int8)
    NVISION_GEN_SCALAR_OPERATORS(int16)
    NVISION_GEN_SCALAR_OPERATORS(int32)
    NVISION_GEN_SCALAR_OPERATORS(int64)

    NVISION_GEN_SCALAR_OPERATORS(uint8)
    NVISION_GEN_SCALAR_OPERATORS(uint16)
    NVISION_GEN_SCALAR_OPERATORS(uint32)
    NVISION_GEN_SCALAR_OPERATORS(uint64)

    NVISION_GEN_SCALAR_OPERATORS(float32)
    NVISION_GEN_SCALAR_OPERATORS(float64)
}

#endif
