/* box_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_BOX_FILTER_H_
#define CVE_BOX_FILTER_H_

#include "cve/src/core/image.h"

namespace cve
{
    /** Filter functor, which applies an iterated box blur to an image.
     *
     *  It applies a 2D average operation on each pixel with kernel:
     *        | 1 1 1 |
     *  1/9 * | 1 1 1 |
     *        | 1 1 1 |
     *
     *  The iterated box filter becomes a Gaussian filter in the limit
     *  (iterations -> inf). */
    template<typename _KernelScalar, Index _Dimension>
    class BoxFilter
    {
    public:
        using KernelScalar = _KernelScalar;
        static constexpr Index Dimension = _Dimension;
        using KernelMatrix = Eigen::Matrix<KernelScalar, Dimension, Dimension>;

        static_assert(Dimension % 2 == 1, "Kernel must have odd dimension");
        static_assert(Eigen::NumTraits<KernelScalar>::IsInteger == 0, "Kernel scalar must be floating point");

        BoxFilter() = default;

        const KernelMatrix &kernel() const
        {
            return _kernel;
        }

        /** Applies the box filter to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the convolution operation */
        template<typename ColorSpace, typename BorderHandling=BorderReflect>
        auto operator()(const Image<ColorSpace> &img, const BorderHandling &handling = BorderHandling{}) const
        {
            return image::correlate(img, _kernel, handling);
        }
    private:
        const KernelMatrix _kernel = KernelMatrix::Constant(KernelScalar{1} / (KernelScalar{Dimension} * KernelScalar{Dimension}));
    };

    template<typename KernelScalar>
    using BoxFilter3 = BoxFilter<KernelScalar, 3>;
    template<typename KernelScalar>
    using BoxFilter5 = BoxFilter<KernelScalar, 5>;
    template<typename KernelScalar>
    using BoxFilter7 = BoxFilter<KernelScalar, 7>;
    template<typename KernelScalar>
    using BoxFilter9 = BoxFilter<KernelScalar, 9>;
}

#endif
