/* clamp.h
 *
 * Author: Fabian Meyer
 * Created On: 10 Sep 2019
 */

#ifndef CVE_CLAMP_H_
#define CVE_CLAMP_H_

#include <cmath>

namespace cve
{
    /** Functor, which clamps a value to a defined interval. */
    template<typename Scalar>
    class Clamp
    {
    private:
        Scalar minval_;
        Scalar maxval_;
    public:
        Clamp(const Scalar minval, const Scalar maxval)
            : minval_(minval), maxval_(maxval)
        { }

        Scalar operator()(const Scalar value) const
        {
            return std::min(maxval_, std::max(minval_, value));
        }
    };

    namespace image
    {
        template<typename Scalar>
        inline Scalar clamp(const Scalar value,
            const Scalar minval,
            const Scalar maxval)
        {
            Clamp<Scalar> tmp(minval, maxval);
            return tmp(value);
        }

        template<typename Scalar>
        inline void clamp(Eigen::Tensor<Scalar, 3> &img,
            const Scalar minval,
            const Scalar maxval)
        {
            img = img.unaryExpr(cve::Clamp<Scalar>(minval, maxval));
        }
    }
}

#endif
