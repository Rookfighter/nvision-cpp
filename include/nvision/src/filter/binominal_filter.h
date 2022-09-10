/* binominal_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 10 Sep 2022
 */

#ifndef NVISION_BINOMINAL_FILTER_H_
#define NVISION_BINOMINAL_FILTER_H_

#include "nvision/src/core/image.h"

namespace nvision
{
    /** Filter functor, which applies Binominal blur to an image. */
    template<typename _KernelScalar, Index _Dimension>
    class BinominalFilter
    {
    public:
        using KernelScalar = _KernelScalar;
        static constexpr Index Dimension = _Dimension;
        using KernelMatrix = Eigen::Matrix<KernelScalar, Dimension, Dimension>;

        static_assert(Eigen::NumTraits<KernelScalar>::IsInteger == 0, "Kernel scalar must be floating point");
        static_assert(Dimension % 2 == 1, "Kernel must have odd dimension");

        BinominalFilter()
            : _kernel(computeKernel())
        {
        }

        const KernelMatrix &kernel() const
        {
            return _kernel;
        }

        /** Applies the binominal filter to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the convolution operation */
        template<typename Derived, typename BorderHandling=BorderReflect>
        auto operator()(const ImageBase<Derived> &img, const BorderHandling &handling = BorderHandling{}) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
            return image::filter<Dimension, Dimension>(img, _kernel, handling);
        }

    private:
        KernelMatrix _kernel = {};

        static constexpr KernelMatrix computeKernel()
        {
            auto vector = Eigen::Matrix<KernelScalar, Dimension, 1>{};
            vector.setOnes();

            for(Index i = 0; i < Dimension - 2; ++i)
            {
                for(Index j = Dimension - i - 2; j < Dimension - 1; ++j)
                {
                    vector(j) += vector(j+1);
                }
            }

            // normalize kernel such that sum of elements is one
            // if it is not normalized, the image becomes darker
            vector /= vector.sum();
            return KernelMatrix{vector * vector.transpose()};
        }
    };

    template<typename _KernelScalar>
    using BinominalFilter3 = BinominalFilter<_KernelScalar, 3>;
    template<typename _KernelScalar>
    using BinominalFilter5 = BinominalFilter<_KernelScalar, 5>;
    template<typename _KernelScalar>
    using BinominalFilter7 = BinominalFilter<_KernelScalar, 7>;
    template<typename _KernelScalar>
    using BinominalFilter9 = BinominalFilter<_KernelScalar, 9>;
}

#endif
