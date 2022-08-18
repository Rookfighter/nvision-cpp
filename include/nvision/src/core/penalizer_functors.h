/* penalizer_functors.h
 *
 * Author: Fabian Meyer
 * Created On: 06 Sep 2019
 */

#ifndef NVISION_PENALIZER_FUNCTORS_H_
#define NVISION_PENALIZER_FUNCTORS_H_

#include <cmath>

namespace nvision
{
    template<typename _Scalar>
    class GaussianPenalizer
    {
    public:
        using Scalar = _Scalar;

        GaussianPenalizer() = default;

        GaussianPenalizer(const Scalar lambda)
            : _lambda(lambda)
        { }

        template<typename Rhs>
        Rhs operator()(const Rhs &value) const
        {
            return nvision::exp(-value / (_lambda * _lambda));
        }
    private:
        Scalar _lambda = Scalar{30};
    };

    template<typename _Scalar>
    class TotalVariationPenalizer
    {
    public:
        using Scalar = _Scalar;

        TotalVariationPenalizer() = default;

        TotalVariationPenalizer(const Scalar eps)
            :_eps(eps)
        { }

        template<typename Rhs>
        Rhs operator()(const Rhs value) const
        {
            return 1 / (2 * ::sqrt(value + _eps));
        }
    private:
        Scalar _eps = static_cast<Scalar>(1e-6);
    };

    template<typename _Scalar>
    class ConstantPenalizer
    {
    public:
        using Scalar = _Scalar;

        ConstantPenalizer() = default;

        ConstantPenalizer(const Scalar constant)
            : _constant(constant)
        { }

        Scalar operator()(const Scalar) const
        {
            return _constant;
        }
    private:
        Scalar _constant = Scalar{1};
    };
}

#endif
