/* canny_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 13 may 2019
 */

#ifndef CVE_CANNY_DETECTOR_H_
#define CVE_CANNY_DETECTOR_H_

#include "cve/core/math.h"
#include "cve/core/image.h"
#include "cve/filter/gauss_filter.h"
#include "cve/filter/sobel_filter.h"

namespace cve
{
    /** Canny edge detection functor. */
    template<typename Scalar,
        typename SmoothFilter=GaussFilter<Scalar>,
        typename GradientFilter=SobelFilter<Scalar>,
        typename BorderHandling=BorderReflect>
    class CannyDetector
    {
    private:
        Scalar lowThreshold_;
        Scalar highThreshold_;
        BorderHandling handling_;

        SmoothFilter smoothFilter_;
        GradientFilter gradientFilter_;


    public:
        CannyDetector()
            : CannyDetector(20, 100)
        {

        }

        CannyDetector(const Scalar lowThreshold, const Scalar highThreshold)
            : lowThreshold_(lowThreshold), highThreshold_(highThreshold),
            handling_(), smoothFilter_(),
            gradientFilter_()
        {
        }

        void setThreshold(const Scalar low, const Scalar high)
        {
            lowThreshold_ = low;
            highThreshold_ = high;
        }

        void setBorderHandling(const BorderHandling &handling)
        {
            handling_ = handling;
        }

        void setSmoothFilter(const SmoothFilter &filter)
        {
            smoothFilter_ = filter;
        }

        void setGradientFilter(const GradientFilter &gradient)
        {
            gradientFilter_ = gradient;
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Tensor<Scalar, 3> gradX;
            Eigen::Tensor<Scalar, 3> gradY;
            Eigen::Tensor<Scalar, 3> gradMag;

            // smooth image
            smoothFilter_(srcImg, gradMag);
            // calculate x and y gradient
            gradientFilter_(destImg, gradX, gradY);
            image::magnitude(gradX, gradY, gradMag);

            // apply non-maximum suppression
            for(Index d = 0; d < gradMag.dimension(2); ++d)
            {
                for(Index c = 0; c < gradMag.dimension(1); ++c)
                {
                    for(Index r = 0; r < gradMag.dimension(0); ++r)
                    {
                        Scalar angle = std::atan2(gradY(r, c, d), gradX(r, c, d));
                        angle += pi<Scalar>();
                        int angIdx = angle / (pi<Scalar>() / 8);

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

                        Scalar valA = handling_(gradMag, rA, cA, d);
                        Scalar valB = handling_(gradMag, rB, cB, d);
                        destImg(r, c, d) = gradMag(r, c, d);
                        // if both neighbours are stronger, suppress this pixel
                        if(gradMag(r, c, d) < valA &&
                            gradMag(r, c, d) < valB)
                        {
                            destImg(r, c, d) = 0;
                        }
                    }
                }
            }

            // perform hysteresis and follow edges
            for(Index d = 0; d < destImg.dimension(2); ++d)
            {
                for(Index c = 0; c < destImg.dimension(1); ++c)
                {
                    for(Index r = 0; r < destImg.dimension(0); ++r)
                    {
                        // if magnitude is below low threshold, suppress this pixel
                        if(destImg(r, c, d) < lowThreshold_)
                        {
                            destImg(r, c, d) = 0;
                        }
                        // perform hyteresis if this is a weak pixel
                        else if(destImg(r, c, d) < highThreshold_)
                        {
                            bool suppress = true;
                            for(Index dc = -1; dc < 2; ++dc)
                            {
                                Index c2 = c + dc;
                                for(Index dr = -1; dr < 2; ++dr)
                                {
                                    Index r2 = r + dr;
                                    ScalarB val = handling_(destImg, r2, c2, d);
                                    if(val >= highThreshold_)
                                    {
                                        suppress = false;
                                        break;
                                    }
                                }
                            }

                            if(suppress)
                                destImg(r, c, d) = 0;
                        }
                    }
                }
            }
        }
    };
}

#endif
