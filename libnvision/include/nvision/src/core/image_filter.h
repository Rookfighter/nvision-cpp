/* image_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 10 Sep 2022
 */

#ifndef NVISION_IMAGE_FILTER_H_
#define NVISION_IMAGE_FILTER_H_

#include "nvision/src/core/image_type.h"
#include "nvision/src/core/image_border_handling.h"

namespace nvision::image
{
    namespace internal
    {
        template<typename Derived, typename KernelType, typename BorderHandling, Index Rows, Index Cols>
        class FilterFunctor
        {
        public:
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be a valid image type");
            static_assert(Eigen::NumTraits<typename KernelType::Scalar>::IsInteger == 0, "kernel must have floating point scalars");

            using KernelScalar = typename KernelType::Scalar;
            static constexpr auto KernelRows = Rows;
            static constexpr auto KernelCols = Cols;
            using PixelType = typename ImageBase<Derived>::Scalar;
            using ColorSpace = typename PixelType::ColorSpace;
            using ImageValueType = typename ColorSpace::ValueType;

            FilterFunctor(const ImageBase<Derived> &img, const KernelType &kernel, const BorderHandling &handling)
            : _img(img), _kernel(kernel), _handling(handling)
            { }

            Pixel<ColorSpace> operator()(const Index row, const Index col) const
            {
                // use the kernel scalars to accumulate
                // these are typically floating points
                Eigen::Array<KernelScalar, ColorSpace::Dimension, 1> result;
                result.setZero();

                for(Index kcol = 0; kcol < KernelCols; ++kcol)
                {
                    const Index vcol = col + kcol - KernelCols / 2;

                    for(Index krow = 0; krow < KernelRows; ++krow)
                    {
                        const Index vrow = row + krow - KernelRows / 2;
                        const auto &pixel = _handling(_img, vrow, vcol);

                        for(Index i = 0; i < ColorSpace::Dimension; ++i)
                            result(i, 0) += _kernel(krow, kcol) * static_cast<KernelScalar>(pixel[i]);
                    }
                }

                // convert the accumulated values back to integral image values
                Pixel<ColorSpace> resultPixel;
                for(Index i = 0; i < ColorSpace::Dimension; ++i)
                    resultPixel[i] = static_cast<ImageValueType>(result(i));

                return resultPixel;
            }

        private:
            const ImageBase<Derived> &_img;
            const KernelType &_kernel;
            const BorderHandling _handling;
        };
    }

    /** Convolves an image with the given kernel and border handling.
      * The kernel must have compile-time dimensions.
      * @param img image which is convolved
      * @param kernel kernel which is used to correlate the image
      * @param handling border handling that is used for convolving
      * @return expression of the convolution */
    template<Index Rows, Index Cols, typename Derived, typename KernelType, typename BorderHandling>
    auto filter(const ImageBase<Derived> &img, const KernelType &kernel, const BorderHandling &handling)
    {
        const auto functor = internal::FilterFunctor<Derived, KernelType, BorderHandling, Rows, Cols>(img, kernel, handling);
        return ImageBase<Derived>::NullaryExpr(img.rows(), img.cols(), functor);
    }

    /** Convolves an image with the given kernel and reflect border handling.
      * The kernel must have compile-time dimensions.
      * @param img image which is convolved
      * @param kernel kernel which is used to correlate the image
      * @return expression of the convolution */
    template<Index Rows, Index Cols, typename Derived, typename KernelType>
    auto filter(const ImageBase<Derived> &img, const KernelType &kernel)
    {
        return filter<Rows, Cols>(img, kernel, BorderReflect{});
    }

}

#endif
