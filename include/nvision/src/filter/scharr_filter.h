/* scharr_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 02 May 2019
 */

#ifndef NVISION_SCHARR_FILTER_H_
#define NVISION_SCHARR_FILTER_H_

#include "nvision/src/core/image.h"
#include "nvision/src/filter/gradient_filter.h"

namespace nvision
{
    /** Filter functor, which computes Scharr gradients of an image.
     *  The Scharr operator computes the first derivative of the image. */
    template<typename _KernelScalar>
    class ScharrFilter
    {
    public:
        using KernelScalar = _KernelScalar;
        static constexpr Index Dimension = 3;
        using KernelMatrix = Eigen::Matrix<KernelScalar, Dimension, Dimension>;

        static_assert(Eigen::NumTraits<KernelScalar>::IsInteger == 0, "Kernel scalar must be floating point");
        static_assert(Dimension % 2 == 1, "Kernel must have odd dimension");

        ScharrFilter()
        {
            _kernelX << -47, 0, 47,
                        -162, 0, 162,
                        -47, 0, 47;
            _kernelY << -47, -162, -47,
                         0,  0,  0,
                         47,  162,  47;
        }

        /** Applies scharr filter in horizontal direction to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the convolution operation */
        template<typename Derived, typename BorderHandling=BorderReflect>
        auto operator()(const ImageBase<Derived> &img,
            const GradientMode::X,
            const BorderHandling &handling = BorderHandling{}) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
            return image::filter<Dimension>(img, _kernelX, handling);
        }

        /** Applies scharr filter in vertical direction to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the convolution operation */
        template<typename Derived, typename BorderHandling=BorderReflect>
        auto operator()(const ImageBase<Derived> &img,
            const GradientMode::Y,
            const BorderHandling &handling = BorderHandling{}) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
            return image::filter<Dimension>(img, _kernelY, handling);
        }
    private:
        KernelMatrix _kernelX = {};
        KernelMatrix _kernelY = {};
    };
}

#endif
