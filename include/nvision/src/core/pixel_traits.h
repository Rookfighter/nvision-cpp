/* pixel_traits.h
 *
 * Author: Fabian Meyer
 * Created On: 23 Dec 2021
 */

#ifndef NVISION_PIXEL_TRAITS_H_
#define NVISION_PIXEL_TRAITS_H_

#include <type_traits>
#include "nvision/src/core/pixel_type.h"
#include "nvision/src/core/color_space.h"

#define NVISION_GEN_NUMTRAITS(C) \
    template<>\
    struct NumTraits<nvision::Pixel<C>>\
    {\
        using ColorSpace = C;\
        using ValueType = typename ColorSpace::ValueType;\
        using Real = ValueType;\
        using NonInteger = nvision::Pixel<ColorSpace>;\
        using Nested = nvision::Pixel<ColorSpace>;\
        using Literal = nvision::Pixel<ColorSpace>;\
        enum {\
            IsInteger = std::is_integral<ValueType>::value ? 1 : 0,\
            IsSigned = std::is_signed<ValueType>::value ? 1 : 0,\
            IsComplex = 0,\
            RequireInitialization = 1,\
            ReadCost = 1,\
            AddCost = 3,\
            MulCost = 3\
        };\
        static constexpr Real epsilon()\
        {\
            return Real(std::numeric_limits<ValueType>::epsilon());\
        }\
        static constexpr Real highest()\
        {\
            return Real(std::numeric_limits<ValueType>::max());\
        }\
         static constexpr Real lowest()\
        {\
            return Real(std::numeric_limits<ValueType>::min());\
        }\
         static constexpr Real min_exponent()\
        {\
            return Real(std::numeric_limits<ValueType>::min_exponent);\
        }\
         static constexpr Real max_exponent()\
        {\
            return Real(std::numeric_limits<ValueType>::max_exponent);\
        }\
        static constexpr Real digits()\
        {\
            return Real(std::numeric_limits<ValueType>::digits);\
        }\
        static constexpr Real digits10()\
        {\
            return Real(std::numeric_limits<ValueType>::digits10);\
        }\
    }

namespace Eigen
{
    NVISION_GEN_NUMTRAITS(nvision::Gray);
    NVISION_GEN_NUMTRAITS(nvision::Grayf);

    NVISION_GEN_NUMTRAITS(nvision::RGB);
    NVISION_GEN_NUMTRAITS(nvision::RGBf);

    NVISION_GEN_NUMTRAITS(nvision::RGBA);
    NVISION_GEN_NUMTRAITS(nvision::RGBAf);

    NVISION_GEN_NUMTRAITS(nvision::BGR);
    NVISION_GEN_NUMTRAITS(nvision::BGRf);

    NVISION_GEN_NUMTRAITS(nvision::BGRA);
    NVISION_GEN_NUMTRAITS(nvision::BGRAf);

    NVISION_GEN_NUMTRAITS(nvision::HSV);

    NVISION_GEN_NUMTRAITS(nvision::YCbCr);
    NVISION_GEN_NUMTRAITS(nvision::YCbCrf);
}

#endif
