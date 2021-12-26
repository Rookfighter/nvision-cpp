/* laplace_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef CVE_LAPLACE_FILTER_H_
#define CVE_LAPLACE_FILTER_H_

#include "cve/src/core/image.h"

namespace cve
{
    /** Filter functor, which computes Laplacian gradients of an image.
      * It computes the sum of the second-order derivatives of the image.
      * Inew = Ixx + Iyy */
    template<typename _KernelScalar>
    class LaplaceFilter
    {
    public:
        using KernelScalar = _KernelScalar;
        static constexpr Index Dimension = 3;
        using KernelMatrix = Eigen::Matrix<KernelScalar, Dimension, Dimension>;

        static_assert(Eigen::NumTraits<KernelScalar>::IsInteger == 0, "Kernel scalar must be floating point");
        static_assert(Dimension % 2 == 1, "Kernel must have odd dimension");

        LaplaceFilter()
        {
            _kernel << 0,  1, 0,
                       1, -4, 1,
                       0,  1, 0;
        }

        const KernelMatrix &kernel() const
        {
            return _kernel;
        }

        /** Applies the laplace filter to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the convolution operation */
        template<typename ColorSpace, typename BorderHandling=BorderReflect>
        auto operator()(const Image<ColorSpace> &img, const BorderHandling &handling = BorderHandling{}) const
        {
            return image::correlate(img, _kernel, handling);
        }

    private:
        KernelMatrix _kernel = {};
    };
}

#endif
