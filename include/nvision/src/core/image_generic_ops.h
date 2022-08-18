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
    template<typename To, typename Derived>
    inline decltype(auto) convert(const ImageBase<Derived> &img)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "input must be a valid image type");
        using PixelType = typename ImageBase<Derived>::Scalar;
        using From = typename PixelType::ColorSpace;
        return img.unaryExpr([](const PixelType &p) { return pixel::convert<From, To>(p); });
    }

    /** Clamps all values in the image to the given interval. */
    template<typename Derived>
    inline decltype(auto) clamp(const ImageBase<Derived> &img,
                                const typename ImageBase<Derived>::Scalar &minval,
                                const typename ImageBase<Derived>::Scalar &maxval)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "input must be a valid image type");
        using PixelType = typename ImageBase<Derived>::Scalar;
        return img.unaryExpr([minval, maxval](const PixelType &pixel) { return pixel::clamp(pixel, minval, maxval); });
    }

    /** Computes the minimum value for each channel in the given image. */
    template<typename Derived>
    inline typename ImageBase<Derived>::Scalar minimum(const ImageBase<Derived> &img)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "input must be a valid image type");
        using ColorSpace = typename ImageBase<Derived>::Scalar::ColorSpace;

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
    template<typename Derived>
    inline typename ImageBase<Derived>::Scalar maximum(const ImageBase<Derived> &img)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "input must be a valid image type");
        using ColorSpace = typename ImageBase<Derived>::Scalar::ColorSpace;

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
    template<typename Derived>
    inline auto normalize(const ImageBase<Derived> &img)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "input must be a valid image type");
        using ColorSpace = typename ImageBase<Derived>::Scalar::ColorSpace;

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
    template<typename Derived>
    inline bool inside(const ImageBase<Derived> &img, const Index row, const Index col)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "input must be a valid image type");

        return row >= 0 && row < img.rows() && col >= 0 && col< img.cols();
    }

    template<typename DerivedLhs, typename DerivedRhs>
    inline auto magnitude(const ImageBase<DerivedLhs> &lhs, const ImageBase<DerivedRhs> &rhs)
    {
        static_assert(IsImage<ImageBase<DerivedLhs>>::value, "lhs input must be a valid image type");
        static_assert(IsImage<ImageBase<DerivedRhs>>::value, "rhs input must be a valid image type");

        return (lhs * lhs + rhs * rhs).sqrt();
    }

    template<typename DerivedLhs, typename DerivedRhs>
    inline auto direction(const ImageBase<DerivedLhs> &lhs, const ImageBase<DerivedRhs> &rhs)
    {
        static_assert(IsImage<ImageBase<DerivedLhs>>::value, "lhs input must be a valid image type");
        static_assert(IsImage<ImageBase<DerivedRhs>>::value, "rhs input must be a valid image type");

        lhs.binaryExpr(rhs, [](const auto &a, const auto &b) { return atan2(a, b); });
    }
}

#endif
