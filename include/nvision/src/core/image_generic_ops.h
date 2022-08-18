/* image_generic_ops.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef NVISION_IMAGE_GENERIC_OPS_H_
#define NVISION_IMAGE_GENERIC_OPS_H_

#include <limits>
#include "nvision/src/core/image_type.h"

namespace nvision::image
{
    /** Converts all pixel within the image to the given colorspace. */
    template<typename From, typename To>
    inline decltype(auto) convert(const Image<From> &img)
    {
        return img.unaryExpr([](const auto &p) { return pixel::convert<From, To>(p); });
    }

    /** Clamps all values in the image to the given interval. */
    template<typename ColorSpace>
    inline decltype(auto) clamp(const Image<ColorSpace> &img, const Pixel<ColorSpace> &minval, const Pixel<ColorSpace> &maxval)
    {
        return img.unaryExpr([minval, maxval](const auto &pixel) { return pixel::clamp(pixel, minval, maxval); });
    }

    /** Computes the minimum value for each channel in the given image. */
    template<typename ColorSpace>
    inline const Pixel<ColorSpace> minimum(const Image<ColorSpace> &img)
    {
        constexpr auto maxval = std::numeric_limits<typename ColorSpace::ValueType>::max();
        auto result = Pixel<ColorSpace>(maxval);

        for(Index c = 0; c < img.cols(); ++c)
        {
            for(Index r = 0; r < img.rows(); ++r)
            {
                const auto &pixel = img(r, c);
                for(Index i = 0; i < pixel.size(); ++i)
                    result[i] = std::min(result[i], pixel[i]);
            }
        }

        return result;
    }

    /** Computes the maximum value for each channel in the given image. */
    template<typename ColorSpace>
    inline const Pixel<ColorSpace> maximum(const Image<ColorSpace> &img)
    {
        constexpr auto minval = std::numeric_limits<typename ColorSpace::ValueType>::min();
        auto result = Pixel<ColorSpace>(minval);

        for(Index c = 0; c < img.cols(); ++c)
        {
            for(Index r = 0; r < img.rows(); ++r)
            {
                const auto &pixel = img(r, c);
                for(Index i = 0; i < pixel.size(); ++i)
                    result[i] = std::max(result[i], pixel[i]);
            }
        }

        return result;
    }

    /** Normalizes each channel of the given image to the given interval [minval, maxval]. */
    template<typename ColorSpace>
    inline auto normalize(const Image<ColorSpace> &img)
    {
        const auto oldMin = minimum(img);
        const auto oldMax = maximum(img);

        // precompute normalization factors
        auto factors = Eigen::Array<float32, ColorSpace::Dimension, 1>();
        for(Index i = 0; i < ColorSpace::Dimension; ++i)
            factors[i] = static_cast<float32>(ColorSpace::maximum[i] - ColorSpace::minimum[i]) / static_cast<float32>(oldMax[i] - oldMin[i]);

        return img.unaryExpr([factors, oldMin, oldMax] (const auto &a) {
            Pixel<ColorSpace> result;
            for(Index i = 0; i < a.size(); ++i)
                result[i] = (a[i] - oldMin[i]) * factors(i) + ColorSpace::minimum[i];

            return result;
        });
    }

    /** Determines if the given row and column lie within the image. */
    template<typename ColorSpace>
    inline bool inside(const Image<ColorSpace> &img, const Index row, const Index col)
    {
        return row >= 0 && row < img.rows() && col >= 0 && col< img.cols();
    }

    template<typename ColorSpace>
    inline auto magnitude(const Image<ColorSpace> &lhs, const Image<ColorSpace> &rhs)
    {
        return (lhs * lhs + rhs * rhs).sqrt();
    }

    template<typename ColorSpace>
    inline auto direction(const Image<ColorSpace> &lhs, const Image<ColorSpace> &rhs)
    {
        lhs.binaryExpr(rhs, [](const auto &a, const auto &b) { return atan2(a, b); });
    }
}

#endif
