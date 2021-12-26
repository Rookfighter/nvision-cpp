/* image_resizing.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_IMAGE_RESIZING_H_
#define CVE_IMAGE_RESIZING_H_

#include "cve/src/core/image_type.h"
#include "cve/src/core/image_interpolation.h"

namespace cve::image
{
    namespace internal
    {
        template<typename ColorSpace, typename Float>
        class ResizeFunctor
        {
        public:
            static_assert(Eigen::NumTraits<Float>::IsInteger == 0, "Resize factor must be floating point");

            ResizeFunctor(const Image<ColorSpace> &img, const Float verticalFactor, const Float horizontalFactor)
            : _img(img), _verticalFactor(verticalFactor), _horizontalFactor(horizontalFactor)
            { }

            const Pixel<ColorSpace> operator()(const Index row, const Index col) const
            {
                const auto origRow = static_cast<Float>(row) / _verticalFactor;
                const auto origCol = static_cast<Float>(col) / _horizontalFactor;
                return interpolateBilinear(_img, origRow, origCol);
            }

        private:
            const Image<ColorSpace> &_img;
            const Float _verticalFactor;
            const Float _horizontalFactor;
        };
    }

    /** Resizes an image by given horizontal and vertical factors and performs up- or downsampling.
      * @param img image to be resized
      * @param horizontalFactor horizontal resize factor, if less than 1  performs downsampling, else upsampling
      * @param verticalFactor horizontal resize factor, if less than 1  performs downsampling, else upsampling
      * @return expression for resize computation */
    template<typename ColorSpace, typename Float>
    inline auto resize(const Image<ColorSpace> &img,
                       const Float horizontalFactor,
                       const Float verticalFactor)
    {
        static_assert(Eigen::NumTraits<Float>::IsInteger == 0, "resize factor must be floating point");

        const auto functor = internal::ResizeFunctor<ColorSpace, Float>(img, verticalFactor, horizontalFactor);
        const auto newHeight = static_cast<Index>(static_cast<Float>(img.rows()) * verticalFactor);
        const auto newWidth = static_cast<Index>(static_cast<Float>(img.cols()) * horizontalFactor);

        return Image<ColorSpace>::NullaryExpr(newHeight, newWidth, functor);
    }

    /** Resizes an image by a uniform factor for horizontal and vertical direction and performs up- or downsampling.
      * @param img image to be resized
      * @param factor resize factor, if less than 1  performs downsampling, else upsampling
      * @return expression for resize computation */
    template<typename ColorSpace, typename Float>
    inline auto resize(const Image<ColorSpace> &img,
                                 const Float factor)
    {
        return resize(img, factor, factor);
    }

    /** Resizes an image to the given size and performs up- or downsampling.
      * @param img image to be resized
      * @param width new width of the image
      * @param height new height of the image
      * @return expression for resize computation */
    template<typename ColorSpace>
    inline auto resize(const Image<ColorSpace> &img,
                                 const Index width,
                                 const Index height)
    {
        const auto verticalFactor = static_cast<float32>(height) / static_cast<float32>(img.rows());
        const auto horizontalFactor = static_cast<float32>(width) / static_cast<float32>(img.cols());
        return resize(img, horizontalFactor, verticalFactor);
    }
}

#endif