/* penalizer_functors.h
 *
 * Author: Fabian Meyer
 * Created On: 06 Sep 2019
 */

#ifndef CVE_PENALIZER_FUNCTORS_H_
#define CVE_PENALIZER_FUNCTORS_H_

#include <cmath>

namespace cve
{
    template<typename Scalar>
    class GaussianPenalizer
    {
    private:
        Scalar lambda_;
    public:
        GaussianPenalizer()
            : GaussianPenalizer(30)
        { }

        GaussianPenalizer(const Scalar lambda)
            : lambda_(lambda)
        { }

        Scalar operator()(const Scalar value) const
        {
            return std::exp(-value / (lambda_ * lambda_));
        }
    };

    template<typename Scalar>
    class TotalVariationPenalizer
    {
    private:
        Scalar eps_;
    public:
        TotalVariationPenalizer()
            : TotalVariationPenalizer(1e-6)
        { }

        TotalVariationPenalizer(const Scalar eps)
            :eps_(eps)
        { }

        Scalar operator()(const Scalar value) const
        {
            return 1 / (2 * std::sqrt(value + eps_));
        }
    };

    template<typename Scalar>
    class ConstantPenalizer
    {
    private:
        Scalar constant_;
    public:
        ConstantPenalizer()
            : ConstantPenalizer(1)
        { }

        ConstantPenalizer(const Scalar constant)
            : constant_(constant)
        { }

        Scalar operator()(const Scalar) const
        {
            return constant_;
        }
    };
}

#endif
