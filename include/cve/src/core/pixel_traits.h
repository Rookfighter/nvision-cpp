/* pixel_traits.h
 *
 * Author: Fabian Meyer
 * Created On: 23 Dec 2021
 */

#ifndef CVE_PIXEL_TRAITS_H_
#define CVE_PIXEL_TRAITS_H_

#include <type_traits>
#include "cve/src/core/pixel_type.h"
#include "cve/src/core/color_space.h"

#define CVE_GEN_NUMTRAITS(C) \
    template<>\
    struct NumTraits<cve::Pixel<C>>\
    {\
        using ColorSpace = C;\
        using ValueType = typename ColorSpace::ValueType;\
        using Real = ValueType;\
        using NonInteger = cve::Pixel<ColorSpace>;\
        using Nested = cve::Pixel<ColorSpace>;\
        using Literal = cve::Pixel<ColorSpace>;\
        enum {\
            IsInteger = std::is_integral<ValueType>::value ? 1 : 0,\
            IsSigned = std::is_signed<ValueType>::value ? 1 : 0,\
            IsComplex = 0,\
            RequireInitialization = 1,\
            ReadCost = 1,\
            AddCost = 3,\
            MulCost = 3\
        };\
        constexpr static Real epsilon()\
        {\
            return Real(std::numeric_limits<ValueType>::epsilon());\
        }\
        constexpr static Real highest()\
        {\
            return Real(std::numeric_limits<ValueType>::max());\
        }\
         constexpr static Real lowest()\
        {\
            return Real(std::numeric_limits<ValueType>::min());\
        }\
         constexpr static Real min_exponent()\
        {\
            return Real(std::numeric_limits<ValueType>::min_exponent);\
        }\
         constexpr static Real max_exponent()\
        {\
            return Real(std::numeric_limits<ValueType>::max_exponent);\
        }\
        constexpr static Real digits()\
        {\
            return Real(std::numeric_limits<ValueType>::digits);\
        }\
        constexpr static Real digits10()\
        {\
            return Real(std::numeric_limits<ValueType>::digits10);\
        }\
    }

namespace Eigen
{
    CVE_GEN_NUMTRAITS(cve::Gray);
    CVE_GEN_NUMTRAITS(cve::Grayf);

    CVE_GEN_NUMTRAITS(cve::RGB);
    CVE_GEN_NUMTRAITS(cve::RGBf);

    CVE_GEN_NUMTRAITS(cve::RGBA);
    CVE_GEN_NUMTRAITS(cve::RGBAf);

    CVE_GEN_NUMTRAITS(cve::BGR);
    CVE_GEN_NUMTRAITS(cve::BGRf);

    CVE_GEN_NUMTRAITS(cve::BGRA);
    CVE_GEN_NUMTRAITS(cve::BGRAf);

    CVE_GEN_NUMTRAITS(cve::HSV);

    CVE_GEN_NUMTRAITS(cve::YCbCr);
    CVE_GEN_NUMTRAITS(cve::YCbCrf);

}

#endif