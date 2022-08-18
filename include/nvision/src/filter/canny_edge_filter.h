/* canny_edge_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 13 may 2019
 */

#ifndef NVISION_CANNY_EDGE_FILTER_H_
#define NVISION_CANNY_EDGE_FILTER_H_

#include "nvision/src/core/image.h"
#include "nvision/src/filter/gauss_filter.h"
#include "nvision/src/filter/sobel_filter.h"

namespace nvision
{
    /** Canny edge detection filter. */
    template<typename _KernelScalar,
             typename _SmoothFilter=GaussFilter<_KernelScalar, 3>,
             typename _GradientFilter=SobelFilter<_KernelScalar>>
    class CannyEdgeFilter
    {
    public:
        using KernelScalar = _KernelScalar;
        using SmoothFilter = _SmoothFilter;
        using GradientFilter = _GradientFilter;

        CannyEdgeFilter() = default;

        CannyEdgeFilter(const KernelScalar low, const KernelScalar high)
            : _lowThreshold(low), _highThreshold(high)
        { }

        void setThreshold(const KernelScalar low, const KernelScalar high)
        {
            _lowThreshold = low;
            _highThreshold = high;
        }

        void setSmoothFilter(const SmoothFilter &filter)
        {
            _smooth = filter;
        }

        void setGradientFilter(const GradientFilter &gradient)
        {
            _gradient = gradient;
        }

        /** Applies the canny edge filter to the given image and returns a expression of the computation.
          * @param img the image on which the box filter should be applied.
          * @param handling the border handling mode; defaults to BorderReflect
          * @return expression of the convolution operation */
        template<typename Derived, typename BorderHandling=BorderReflect>
        auto operator()(const ImageBase<Derived> &img, const BorderHandling &handling = {}) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
            using ColorSpace = ImageBase<Derived>::Scalar::ColorSpace;

            // smooth image
            Image<ColorSpace> result = _smooth(img);
            // calculate x and y gradient
            Image<ColorSpace> gradX = _gradient(result, GradientMode::X());
            Image<ColorSpace> gradY = _gradient(result, GradientMode::Y());
            Image<ColorSpace> gradMag = image::magnitude(gradX, gradY);

            // apply non-maximum suppression
            for(Index c = 0; c < gradMag.cols(); ++c)
            {
                for(Index r = 0; r < gradMag.rows(); ++r)
                {
                    for(Index d = 0; d < ColorSpace::Dimension; ++d)
                    {
                        const auto dx = static_cast<KernelScalar>(gradX(r, c)[d]);
                        const auto dy = static_cast<KernelScalar>(gradY(r, c)[d]);
                        const auto angle = std::atan2(dy, dx) + pi<KernelScalar>();
                        const auto angIdx = static_cast<Index>(angle / (pi<KernelScalar>() / 8));

                        Index rA;
                        Index cA;
                        Index rB;
                        Index cB;

                        // edge points in east-west and west-east
                        if(angIdx == 0 || angIdx == 15 || angIdx == 7 || angIdx == 8)
                        {
                            rA = r + 1;
                            cA = c;
                            rB = r - 1;
                            cB = c;
                        }
                        // edge points in north-south and south-north
                        else if(angIdx == 3 || angIdx == 4 || angIdx == 11 || angIdx == 12)
                        {
                            rA = r;
                            cA = c + 1;
                            rB = r;
                            cB = c - 1;
                        }
                        // edge points in northeast-southwest and southwest-northeast
                        else if(angIdx == 1 || angIdx == 2 || angIdx == 9 || angIdx == 10)
                        {
                            rA = r + 1;
                            cA = c - 1;
                            rB = r - 1;
                            cB = c + 1;
                        }
                        // edge points in northwest-southeast and southeast-northwest
                        else
                        {
                            rA = r + 1;
                            cA = c + 1;
                            rB = r - 1;
                            cB = c - 1;
                        }

                        const auto val = gradMag(r, c)[d];
                        const auto valA = handling(gradMag, rA, cA)[d];
                        const auto valB = handling(gradMag, rB, cB)[d];
                        const auto strongNeighs = val < valA && val < valB;
                        // if both neighbours are stronger, suppress this pixel
                        result(r, c)[d] = strongNeighs ? 0 : gradMag(r, c)[d];
                    }
                }
            }

            // perform hysteresis and follow edges
            for(Index c = 0; c < result.cols(); ++c)
            {
                for(Index r = 0; r < result.rows(); ++r)
                {
                    for(Index d = 0; d < ColorSpace::Dimension; ++d)
                    {
                        // if magnitude is below low threshold, suppress this pixel
                        if(result(r, c)[d] < _lowThreshold)
                        {
                            result(r, c)[d] = 0;
                        }
                        // perform hyteresis if this is a weak pixel
                        else if(result(r, c)[d] < _highThreshold)
                        {
                            bool suppress = true;
                            for(Index dc = -1; dc < 2; ++dc)
                            {
                                const auto c2 = c + dc;
                                for(Index dr = -1; dr < 2; ++dr)
                                {
                                    const auto r2 = r + dr;
                                    const auto val = handling(result, r2, c2)[d];
                                    if(val >= _highThreshold)
                                    {
                                        suppress = false;
                                        break;
                                    }
                                }
                            }

                            if(suppress)
                            {
                                result(r, c)[d] = 0;
                            }
                        }
                    }
                }
            }

            return result;
        }

    private:
        KernelScalar _lowThreshold = static_cast<KernelScalar>(0.078f);
        KernelScalar _highThreshold = static_cast<KernelScalar>(0.3992f);

        SmoothFilter _smooth = {};
        GradientFilter _gradient = {};
    };
}

#endif
