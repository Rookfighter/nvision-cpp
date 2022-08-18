/* pixel_generic_ops.h
 *
 * Author: Fabian Meyer
 * Created On: 23 Dec 2021
 */

#ifndef NVISION_PIXEL_GENERIC_OPS_H_
#define NVISION_PIXEL_GENERIC_OPS_H_

#include "nvision/src/core/pixel_type.h"
#include "nvision/src/core/math.h"

namespace nvision
{
    namespace pixel
    {
        template<typename ColorSpace, typename F>
        Pixel<ColorSpace> unaryOperation(const Pixel<ColorSpace> &lhs, F &&op)
        {
            Pixel<ColorSpace> result;
            for(Index i = 0; i < result.size(); ++i)
                result[i] = op(lhs[i]);
            return result;
        }

        template<typename ColorSpace, typename F>
        Pixel<ColorSpace> binaryOperation(const Pixel<ColorSpace> &lhs, const Pixel<ColorSpace> &rhs, F &&op)
        {
            Pixel<ColorSpace> result;
            for(Index i = 0; i < result.size(); ++i)
                result[i] = op(lhs[i], rhs[i]);
            return result;
        }

        template<typename ColorSpace>
        inline Pixel<ColorSpace> clamp(const Pixel<ColorSpace> &rhs, const Pixel<ColorSpace> &minval, const Pixel<ColorSpace> &maxval)
        {
            Pixel<ColorSpace> result;
            for(Index i = 0; i < result.size(); ++i)
                result[i] = clamp(rhs[i], minval[i], maxval[i]);
            return result;
        }

        template<typename ColorSpaceIn, typename ColorSpaceOut>
        inline Pixel<ColorSpaceOut> convertRange(const Pixel<ColorSpaceIn> &pixel)
        {
            static_assert(ColorSpaceIn::Dimension == ColorSpaceOut::Dimension, "Color spaces must have same dimension");

            using ValueTypeOut = typename ColorSpaceOut::ValueType;

            Pixel<ColorSpaceOut> result;
            for(Index i = 0; i < ColorSpaceIn::Dimension; ++i)
            {
                const auto minIn  = static_cast<float32>(ColorSpaceIn::minimum[i]);
                const auto minOut = static_cast<float32>(ColorSpaceOut::minimum[i]);
                const auto maxIn  = static_cast<float32>(ColorSpaceIn::maximum[i]);
                const auto maxOut = static_cast<float32>(ColorSpaceOut::maximum[i]);
                result[i] = static_cast<ValueTypeOut>(((pixel[i] - minIn) / (maxIn - minIn)) * (maxOut - minOut) + minOut);
            }

            return result;
        }
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> sqrt(const Pixel<ColorSpace> &rhs)
    {
        using ValueType = typename ColorSpace::ValueType;
        return pixel::unaryOperation(rhs, [](const ValueType value) { return std::sqrt(value); });
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> abs(const Pixel<ColorSpace> &rhs)
    {
        using ValueType = typename ColorSpace::ValueType;
        return pixel::unaryOperation(rhs, [](const ValueType value) { return std::abs(value); });
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> log10(const Pixel<ColorSpace> &rhs)
    {
        using ValueType = typename ColorSpace::ValueType;
        return pixel::unaryOperation(rhs, [](const ValueType value) { return std::log10(value); });
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> log(const Pixel<ColorSpace> &rhs)
    {
        using ValueType = typename ColorSpace::ValueType;
        return pixel::unaryOperation(rhs, [](const ValueType value) { return std::log(value); });
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> log2(const Pixel<ColorSpace> &rhs)
    {
        using ValueType = typename ColorSpace::ValueType;
        return pixel::unaryOperation(rhs, [](const ValueType value) { return std::log2(value); });
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> exp(const Pixel<ColorSpace> &rhs)
    {
        using ValueType = typename ColorSpace::ValueType;
        return pixel::unaryOperation(rhs, [](const ValueType value) { return std::exp(value); });
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> ceil(const Pixel<ColorSpace> &rhs)
    {
        using ValueType = typename ColorSpace::ValueType;
        return pixel::unaryOperation(rhs, [](const ValueType value) { return std::ceil(value); });
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> floor(const Pixel<ColorSpace> &rhs)
    {
        using ValueType = typename ColorSpace::ValueType;
        return pixel::unaryOperation(rhs, [](const ValueType value) { return std::floor(value); });
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> round(const Pixel<ColorSpace> &rhs)
    {
        using ValueType = typename ColorSpace::ValueType;
        return pixel::unaryOperation(rhs, [](const ValueType value) { return std::round(value); });
    }

    template<typename ColorSpace>
    inline Pixel<ColorSpace> atan2(const Pixel<ColorSpace> &lhs, const Pixel<ColorSpace> &rhs)
    {
        return pixel::binaryOperation(lhs, rhs, [](const auto a, const auto b) { return std::atan2(a, b); });
    }
}


#endif
