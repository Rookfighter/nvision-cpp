/* resize_image.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Sep 2019
 */

#ifndef CVE_RESIZE_IMAGE_H_
#define CVE_RESIZE_IMAGE_H_

#include "cve/core/bilinear_interpolation.h"

namespace cve
{
    /** Functor which resizes a given image by a given factor.
      * This functor basically performs up- or downsampling based on bilinear
      * interpolation. */
    template<typename Scalar,
        typename Interpolation=BilinearInterpolation<Scalar>>
    class ResizeImage
    {
    private:
        Interpolation interpolation_;
    public:
        ResizeImage()
            : interpolation_()
        { }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Scalar factor,
            const Eigen::Tensor<ScalarA, 3> &src,
            Eigen::Tensor<ScalarB, 3> &dest) const
        {
            Index height = factor * src.dimension(0);
            Index width = factor * src.dimension(1);

            dest.resize(height, width, src.dimension(2));

            for(Index d = 0; d < dest.dimension(2); ++d)
            {
                for(Index c = 0; c < dest.dimension(1); ++c)
                {
                    for(Index r = 0; r < dest.dimension(0); ++r)
                    {
                        Scalar x = static_cast<Scalar>(c) / factor;
                        Scalar y = static_cast<Scalar>(r) / factor;

                        dest(r, c, d) = interpolation_(src, y, x, d);
                    }
                }
            }
        }
    };

    namespace image
    {
        /** Resizes an image by a given factor and performs up- or downsampling.
          * @see ResizeImage
          * @param factor resize factor, if < 1 the downsample, else upsample
          * @param src source image
          * @param dest output image */
        template<typename Scalar, typename ScalarA, typename ScalarB>
        inline void resize(const Scalar factor,
            const Eigen::Tensor<ScalarA, 3> &src,
            Eigen::Tensor<ScalarB, 3> &dest)
        {
            ResizeImage<Scalar> resizeimg;
            return resizeimg(factor, src, dest);
        }
    }
}

#endif
