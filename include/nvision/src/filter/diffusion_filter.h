/* diffusion_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 05 Sep 2019
 */

#ifndef NVISION_DIFFUSION_FILTER_H_
#define NVISION_DIFFUSION_FILTER_H_

#include "nvision/src/filter/central_differences_filter.h"
#include "nvision/src/core/penalizer_functors.h"

namespace nvision
{
    template<typename _KernelScalar,
        typename Penalizer=GaussianPenalizer<_KernelScalar>,
        typename GradientFilter=CentralDifferencesFilter<_KernelScalar>>
    class DiffusionFilter
    {
    public:
        using KernelScalar = _KernelScalar;

        static_assert(Eigen::NumTraits<KernelScalar>::IsInteger == 0, "Kernel scalar must be floating point");

        DiffusionFilter() = default;

        DiffusionFilter(const Index iterations, const KernelScalar flowFactor)
            : _iterations(iterations), _flowFac(flowFactor)
        { }

        void setMaxIterations(const Index iterations)
        {
            _iterations = iterations;
        }

        void setFlowFactor(const KernelScalar flowFactor)
        {
            _flowFac = flowFactor;
        }

        void setGradientFilter(const GradientFilter &filter)
        {
            _gradient = filter;
        }

        void setPenalizer(const Penalizer &penalizer)
        {
            _penalizer = penalizer;
        }

        /** Applies the diffusion filter to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the filter application */
        template<typename ColorSpace, typename BorderHandling=BorderReflect>
        auto operator()(const Image<ColorSpace> &img, const BorderHandling &handling = {}) const
        {
            static_assert(Eigen::NumTraits<typename ColorSpace::ValueType>::IsInteger == 0, "Image must use floating point value");

            Image<ColorSpace> u = img;
            Image<ColorSpace> ux;
            Image<ColorSpace> uy;

            Image<ColorSpace> g;
            Image<ColorSpace> gux;
            Image<ColorSpace> guy;
            Image<ColorSpace> guxx;
            Image<ColorSpace> guyy;

            Image<ColorSpace> tmp;
            for(Index i = 0; i < _iterations; ++i)
            {
                tmp = u;
                ux = _gradient(tmp, GradientMode::X(), handling);
                uy = _gradient(tmp, GradientMode::Y(), handling);

                g = (ux * ux + uy * uy).unaryExpr(_penalizer);

                gux = g * ux;
                guy = g * uy;

                guxx = _gradient(gux, GradientMode::X(), handling);
                guyy = _gradient(guy, GradientMode::Y(), handling);

                g = guxx + guyy;
                u += g * _flowFac;
            }

            return u;
        }

    private:
        GradientFilter _gradient = {};
        Penalizer _penalizer = {};

        Index _iterations = 10;
        KernelScalar _flowFac = static_cast<KernelScalar>(0.05);
    };
}

#endif
