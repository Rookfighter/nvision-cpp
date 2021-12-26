/* image_correlation.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_IMAGE_CORRELATION_H_
#define CVE_IMAGE_CORRELATION_H_

#include "cve/src/core/image_type.h"
#include "cve/src/core/image_border_handling.h"

namespace cve::image
{
    namespace internal
    {
        template<typename ColorSpace, typename KernelType, typename BorderHandling>
        class CorrelateFunctor
        {
        public:
            using KernelScalar = typename KernelType::Scalar;
            using ImageValueType = typename ColorSpace::ValueType;

            static_assert(Eigen::NumTraits<KernelScalar>::IsInteger == 0, "kernel must have floating point scalars");

            CorrelateFunctor(const Image<ColorSpace> &img, const KernelType &kernel, const BorderHandling &handling)
            : _img(img), _kernel(kernel), _handling(handling)
            { }

            Pixel<ColorSpace> operator()(const Index row, const Index col) const
            {
                // use the kernel scalars to accumulate
                // these are typically floating points
                Eigen::Array<KernelScalar, ColorSpace::Dimension, 1> result;
                result.setZero();

                for(Index kcol = 0; kcol < _kernel.cols(); ++kcol)
                {
                    const Index vcol = col + kcol - _kernel.cols() / 2;

                    for(Index krow = 0; krow < _kernel.rows(); ++krow)
                    {
                        const Index vrow = row + krow - _kernel.rows() / 2;
                        const auto &pixel = _handling(_img, vrow, vcol);

                        for(Index i = 0; i < ColorSpace::Dimension; ++i)
                            result(i, 0) += _kernel(krow, kcol) * static_cast<KernelScalar>(pixel[i]);
                    }
                }

                // convert the accumulated values back to integral image values
                Pixel<ColorSpace> resultPixel;
                for(Index i = 0; i < result.size(); ++i)
                    resultPixel[i] = static_cast<ImageValueType>(result(i));

                return resultPixel;
            }

        private:
            const Image<ColorSpace> &_img;
            const KernelType &_kernel;
            const BorderHandling _handling;
        };
    }

    /** Convolves an image with the given kernel and border handling.
      * @param img image which is convolved
      * @param kernel kernel which is used to correlate the image
      * @param handling border handling that is used for convolving
      * @return expression of the convolution */
    template<typename ColorSpace, typename KernelType, typename BorderHandling>
    auto correlate(const Image<ColorSpace> &img, const KernelType &kernel, const BorderHandling &handling)
    {
        const auto functor = internal::CorrelateFunctor<ColorSpace, KernelType, BorderHandling>(img, kernel, handling);
        return Image<ColorSpace>::NullaryExpr(img.rows(), img.cols(), functor);
    }

    /** Convolves an image with the given kernel and reflect border handling.
      * @param img image which is convolved
      * @param kernel kernel which is used to correlate the image
      * @return expression of the convolution */
    template<typename ColorSpace, typename KernelType>
    auto correlate(const Image<ColorSpace> &img, const KernelType &kernel)
    {
        return correlate(img, kernel, BorderReflect{});
    }

}

#endif