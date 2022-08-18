/* gauss_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef NVISION_GAUSS_FILTER_H_
#define NVISION_GAUSS_FILTER_H_

#include "nvision/src/core/image.h"

namespace nvision
{
    /** Filter functor, which applies Gaussian blur to an image.
     *  A kernel size which is greater than ceil(6 * sigma) brings no advantage
     *  in precision. */
    template<typename _KernelScalar, Index _Dimension>
    class GaussFilter
    {
    public:
        using KernelScalar = _KernelScalar;
        static constexpr Index Dimension = _Dimension;
        using KernelMatrix = Eigen::Matrix<KernelScalar, Dimension, Dimension>;

        static_assert(Eigen::NumTraits<KernelScalar>::IsInteger == 0, "Kernel scalar must be floating point");
        static_assert(Dimension % 2 == 1, "Kernel must have odd dimension");

        GaussFilter()
            : GaussFilter(KernelScalar{1})
        { }

        GaussFilter(const KernelScalar sigma)
            : _sigma(sigma)
        {
            computeKernel();
        }

        KernelScalar sigma() const
        {
            return _sigma;
        }

        const KernelMatrix &kernel() const
        {
            return _kernel;
        }

        /** Applies the gauss filter to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the convolution operation */
        template<typename Derived, typename BorderHandling=BorderReflect>
        auto operator()(const ImageBase<Derived> &img, const BorderHandling &handling = BorderHandling{}) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
            return image::correlate(img, _kernel, handling);
        }

    private:
        const KernelScalar _sigma = KernelScalar{1};
        KernelMatrix _kernel = {};

        void computeKernel()
        {
            constexpr auto rowHalf = Dimension / 2;
            Eigen::Matrix<KernelScalar, Dimension, 1> vector;

            for(Index i = 0; i < rowHalf + 1; ++i)
            {
                const auto idxA = rowHalf - i;
                const auto idxB = rowHalf + i;

                const auto numerator = static_cast<KernelScalar>(i) * static_cast<KernelScalar>(i);
                const auto denominator = KernelScalar{2} * _sigma * _sigma;
                // omit gauss normalization factor
                // the kernel is normalized after the loop
                const auto value = std::exp(-numerator / denominator);

                // the kernel is symmetric
                vector(idxA) = value;
                vector(idxB) = value;
            }

            // normalize kernel such that sum of elements is one
            // if it is not normalized, the image becomes darker
            vector /= vector.sum();
            _kernel = vector * vector.transpose();
        }
    };

    template<typename _KernelScalar>
    using GaussFilter3 = GaussFilter<_KernelScalar, 3>;
    template<typename _KernelScalar>
    using GaussFilter5 = GaussFilter<_KernelScalar, 5>;
    template<typename _KernelScalar>
    using GaussFilter7 = GaussFilter<_KernelScalar, 7>;
    template<typename _KernelScalar>
    using GaussFilter9 = GaussFilter<_KernelScalar, 9>;
}

#endif
