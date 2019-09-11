/* clamp.h
 *
 * Author: Fabian Meyer
 * Created On: 10 Sep 2019
 */

#ifndef CVE_CLAMP_H_
#define CVE_CLAMP_H_

#include "cve/core/math.h"

namespace cve
{
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
            return cve::clamp(value, minval_, maxval_);
        }
    };

    namespace image
    {
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
