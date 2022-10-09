/* math.h
 *
 * Author: Fabian Meyer
 * Created On: 15 May 2019
 */

#ifndef NVISION_MATH_H_
#define NVISION_MATH_H_

#include <cmath>
#include <type_traits>

namespace nvision
{
    template<typename Scalar>
    constexpr Scalar pi()
    {
        static_assert(std::is_floating_point<Scalar>::value, "pi must be floating point");
        return static_cast<Scalar>(3.1415926535897932);
    }

    template<typename Scalar>
    inline Scalar clamp(const Scalar value, const Scalar minval, const Scalar maxval)
    {
        return std::min(maxval, std::max(minval, value));
    }

    namespace angle
    {
        template<typename Scalar>
        inline Scalar normalize(const Scalar angle)
        {
            Scalar result = angle;
            while(result >= 2 * pi<Scalar>())
                result -= 2 * pi<Scalar>();
            while(result < 0)
                result += 2 * pi<Scalar>();
            return result;
        }

        template<typename Scalar>
        inline Scalar radian2degree(const Scalar angle)
        {
            return angle * Scalar{180} / pi<Scalar>();
        }

        template<typename Scalar>
        inline Scalar degree2radian(const Scalar angle)
        {
            return angle * pi<Scalar>() / Scalar{180};
        }
    }
}

#endif
