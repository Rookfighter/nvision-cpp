/* forward_differences_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef NVISION_FORWARD_DIFFERENCES_FILTER_H_
#define NVISION_FORWARD_DIFFERENCES_FILTER_H_

#include "nvision/src/core/image.h"
#include "nvision/src/filter/gradient_filter.h"

namespace nvision
{
    /** Filter functor to compute backward differences gradients. */
    template<typename _KernelScalar>
    class ForwardDifferencesFilter
    {
    public:
        using KernelScalar = _KernelScalar;
        static constexpr Index Dimension = 3;
        using KernelMatrix = Eigen::Matrix<KernelScalar, Dimension, Dimension>;

        static_assert(Eigen::NumTraits<KernelScalar>::IsInteger == 0, "Kernel scalar must be floating point");

        /** Applies forward differences in horizontal direction to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the convolution operation */
        template<typename ColorSpace, typename BorderHandling=BorderReflect>
        auto operator()(const Image<ColorSpace> &img,
            const GradientMode::X,
            const BorderHandling &handling = BorderHandling{}) const
        {
            return image::correlate(img, _kernelX, handling);
        }

        /** Applies forward differences in vertical direction to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the convolution operation */
        template<typename ColorSpace, typename BorderHandling=BorderReflect>
        auto operator()(const Image<ColorSpace> &img,
            const GradientMode::Y,
            const BorderHandling &handling = BorderHandling{}) const
        {
            return image::correlate(img, _kernelY, handling);
        }
    private:
        const Eigen::Matrix<KernelScalar, 1, 3> _kernelX{0, -1, 1};
        const Eigen::Matrix<KernelScalar, 3, 1> _kernelY{0, -1, 1};
    };
}

#endif
