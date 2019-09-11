/* image.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_IMAGE_H_
#define CVE_IMAGE_H_

#include "cve/core/bilinear_interpolation.h"
#include "cve/core/downsample.h"
#include "cve/core/upsample.h"
#include "cve/core/clamp.h"

namespace cve
{
    typedef Eigen::Tensor<uint8_t, 3> Image8;
    typedef Eigen::Tensor<uint16_t, 3> Image16;
    typedef Eigen::Tensor<uint32_t, 3> Image32;
    typedef Eigen::Tensor<float, 3> Imagef;
    typedef Eigen::Tensor<double, 3> Imaged;

    typedef Eigen::Tensor<uint8_t, 1> Pixel8;
    typedef Eigen::Tensor<uint16_t, 1> Pixel16;
    typedef Eigen::Tensor<uint32_t, 1> Pixel32;
    typedef Eigen::Tensor<float, 1> Pixelf;
    typedef Eigen::Tensor<double, 1> Pixeld;

    namespace image
    {
        template<typename Scalar>
        void normalize(Eigen::Tensor<Scalar, 3> &img,
            const Scalar minval,
            const Scalar maxval)
        {
            Eigen::Tensor<Scalar, 1> oldMin = img.minimum(Eigen::array<int, 2>({0, 1}));
            Eigen::Tensor<Scalar, 1> oldMax = img.maximum(Eigen::array<int, 2>({0, 1}));

            assert(oldMin.dimension(0) == img.dimension(2));
            assert(oldMax.dimension(0) == img.dimension(2));

            for(Index d = 0; d < img.dimension(2); ++d)
            {
                for(Index c = 0; c < img.dimension(1); ++c)
                {
                    for(Index r = 0; r < img.dimension(0); ++r)
                    {
                        Scalar factor = (maxval - minval) / (oldMax(d) - oldMin(d));
                        img(r, c, d) = (img(r, c, d) - oldMin(d)) * factor + minval;
                    }
                }
            }
        }

        template<typename Scalar>
        void gray2rgb(const Eigen::Tensor<Scalar, 3> &src,
            Eigen::Tensor<Scalar, 3> &dest)
        {
            dest.resize(src.dimension(0), src.dimension(1), 3);

            for(Index c = 0; c < src.dimension(1); ++c)
            {
                for(Index r = 0; r < src.dimension(0); ++r)
                {
                    dest(r, c, 0) = src(r, c, 0);
                    dest(r, c, 1) = src(r, c, 0);
                    dest(r, c, 2) = src(r, c, 0);
                }
            }
        }

        template<typename Scalar>
        void rgb2gray(const Eigen::Tensor<Scalar, 3> &src,
            Eigen::Tensor<Scalar, 3> &dest)
        {
            assert(src.dimension(2) >= 3);
            dest.resize(src.dimension(0), src.dimension(1), 1);

            for(Index c = 0; c < src.dimension(1); ++c)
            {
                for(Index r = 0; r < src.dimension(0); ++r)
                {
                    dest(r, c, 0) = 0.2126 * src(r, c, 0) +
                        0.7152 * src(r, c, 1) + 0.0722 * src(r, c, 2);
                }
            }
        }

        template<typename Scalar>
        bool isInside(const Index row, const Index col,
            const Eigen::Tensor<Scalar, 3> &img)
        {
            return row >= 0 && row < img.dimension(0) &&
                col >= 0 && col < img.dimension(1);
        }

        template<typename ScalarA, typename ScalarB>
        void magnitude(const Eigen::Tensor<ScalarA, 3> &x,
            const Eigen::Tensor<ScalarA, 3> &y,
            Eigen::Tensor<ScalarB, 3> &magnitude)
        {
            magnitude = (x * x + y * y).sqrt();
        }

        template<typename ScalarA, typename ScalarB>
        void magnitudeSq(const Eigen::Tensor<ScalarA, 3> &x,
            const Eigen::Tensor<ScalarA, 3> &y,
            Eigen::Tensor<ScalarB, 3> &magnitude)
        {
            magnitude = x * x + y * y;
        }

        template<typename ScalarA, typename ScalarB>
        void magnitude(const Eigen::Tensor<ScalarA, 3> &img,
            Eigen::Tensor<ScalarB, 3> &magnitude)
        {
            magnitude = (img * img).sum(Eigen::array<int, 1>({2})).sqrt();
        }

        template<typename ScalarA, typename ScalarB>
        void magnitudeSq(const Eigen::Tensor<ScalarA, 3> &img,
            Eigen::Tensor<ScalarB, 3> &magnitude)
        {
            magnitude = (img * img).sum(Eigen::array<int, 2>({0, 1}));
        }

        template<typename ScalarA, typename ScalarB>
        void direction(const Eigen::Tensor<ScalarA, 3> &x,
            const Eigen::Tensor<ScalarA, 3> &y,
            Eigen::Tensor<ScalarB, 3> &direction)
        {
            direction.resize(x.dimensions());
            for(Index d = 0; d < x.dimension(2); ++d)
                for(Index c = 0; c < x.dimension(1); ++c)
                    for(Index r = 0; r < x.dimension(0); ++r)
                        direction(r, c, d) = std::atan2(y(r, c, d), x(r, c, d));
        }

        template<typename ScalarA, typename ScalarB>
        void direction(const Eigen::Tensor<ScalarA, 3> &img,
            Eigen::Tensor<ScalarB, 3> &direction)
        {
            assert(img.dimension(2) == 2);

            direction.resize(img.dimension(0), img.dimension(1), 1);
            for(Index c = 0; c < img.dimension(1); ++c)
                for(Index r = 0; r < img.dimension(0); ++r)
                        direction(r, c, 0) = std::atan2(img(r, c, 1), img(r, c, 0));
        }
    }
}

#endif
