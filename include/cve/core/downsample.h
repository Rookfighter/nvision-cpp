/* downsample.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Sep 2019
 */

#ifndef CVE_DOWNSAMPLE_H_
#define CVE_DOWNSAMPLE_H_

#include "cve/core/bilinear_interpolation.h"

namespace cve
{
    template<typename Scalar,
        typename Interpolator=BilinearInterpolator<Scalar>>
    class Downsampler
    {
    private:
        Interpolator interpolator_;
    public:
        Downsampler()
            : interpolator_()
        { }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Scalar factor,
            const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            assert(factor > 0);
            assert(factor < 1);

            Index height = factor * srcImg.dimension(0);
            Index width = factor * srcImg.dimension(1);

            destImg.resize(height, width, srcImg.dimension(2));

            for(Index d = 0; d < destImg.dimension(2); ++d)
            {
                for(Index c = 0; c < destImg.dimension(1); ++c)
                {
                    for(Index r = 0; r < destImg.dimension(0); ++r)
                    {
                        Scalar x = static_cast<Scalar>(c) / factor;
                        Scalar y = static_cast<Scalar>(r) / factor;

                        destImg(r, c, d) = interpolator_(srcImg, y, x, d);
                    }
                }
            }
        }
    };

    namespace image
    {
        template<typename Scalar, typename ScalarA, typename ScalarB>
        inline void downsample(const Scalar factor,
            const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg)
        {
            Downsampler<Scalar> downsampler;
            return downsampler(factor, srcImg, destImg);
        }
    }
}

#endif
