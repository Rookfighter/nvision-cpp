/* canny_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 13 may 2019
 */

#ifndef CVE_CANNY_DETECTOR_H_
#define CVE_CANNY_DETECTOR_H_

#include "cve/math.h"
#include "cve/filter/gauss_filter.h"
#include "cve/filter/sobel_filter.h"

namespace cve
{
    /** Class for Canny edge detection. */
    template<typename Scalar,
        typename SmoothFilter=GaussFilter<Scalar, 5>,
        typename GradientFilter=SobelFilter<Scalar>>
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
            handling_(BorderHandling::Reflect), smoothFilter_(),
            gradientFilter_()
        {
        }

        void setThreshold(const Scalar low, const Scalar high)
        {
            lowThreshold_ = low;
            highThreshold_ = high;
        }

        void setSmoothFilter(const SmoothFilter &filter)
        {
            smoothFilter_ = filter;
        }

        void setGradientFilter(const GradientFilter &gradient)
        {
            gradientFilter_ = gradient;
        }

        template<typename ImageA, typename ImageB>
        void apply(const ImageA &srcImg, ImageB &destImg) const
        {
            ImageB xgrad;
            ImageB ygrad;
            ImageB maggrad;

            // smooth image
            smoothFilter_.apply(srcImg, destImg);
            // calculate x and y gradient
            gradientFilter_.applyX(destImg, xgrad);
            gradientFilter_.applyY(destImg, ygrad);

            maggrad = (xgrad * xgrad + ygrad * ygrad).sqrt();

            // apply non-maximum suppression
            for(Index c = 0; c < maggrad.cols(); ++c)
            {
                for(Index r = 0; r < maggrad.rows(); ++r)
                {
                    for(Index d = 0; d < maggrad(r, c).size(); ++d)
                    {
                        Scalar angle = std::atan2(ygrad(r, c)(d), xgrad(r, c)(d));
                        angle += pi<Scalar>();
                        int angIdx = angle / (pi<Scalar>() / 8);

                        Index rA = r;
                        Index cA = c;
                        Index rB = r;
                        Index cB = c;

                        // edge points in east-west and west-east
                        if(angIdx == 0 || angIdx == 15 || angIdx == 7 || angIdx == 8)
                        {
                            rA = border::handle(r + 1, 0, maggrad.rows(), handling_);
                            rB = border::handle(r - 1, 0, maggrad.rows(), handling_);
                        }
                        // edge points in north-south and south-north
                        else if(angIdx == 3 || angIdx == 4 || angIdx == 11 || angIdx == 12)
                        {
                            cA = border::handle(c + 1, 0, maggrad.cols(), handling_);
                            cB = border::handle(c - 1, 0, maggrad.cols(), handling_);
                        }
                        // edge points in northeast-southwest and southwest-northeast
                        else if(angIdx == 1 || angIdx == 2 || angIdx == 9 || angIdx == 10)
                        {
                            rA = border::handle(r + 1, 0, maggrad.rows(), handling_);
                            cA = border::handle(c - 1, 0, maggrad.cols(), handling_);
                            rB = border::handle(r - 1, 0, maggrad.rows(), handling_);
                            cB = border::handle(c + 1, 0, maggrad.cols(), handling_);
                        }
                        // edge points in northwest-southeast and southeast-northwest
                        else
                        {
                            rA = border::handle(r + 1, 0, maggrad.rows(), handling_);
                            cA = border::handle(c + 1, 0, maggrad.cols(), handling_);
                            rB = border::handle(r - 1, 0, maggrad.rows(), handling_);
                            cB = border::handle(c - 1, 0, maggrad.cols(), handling_);
                        }

                        destImg(r, c)(d) = maggrad(r, c)(d);
                        // if both neighbours are stronger, suppress this pixel
                        if(maggrad(r, c)(d) < maggrad(rA, cA)(d) &&
                            maggrad(r, c)(d) < maggrad(rB, cB)(d))
                        {
                            destImg(r, c)(d) = 0;
                        }
                    }
                }
            }

            // perform hysteresis and follow edges
            for(Index c = 0; c < destImg.cols(); ++c)
            {
                for(Index r = 0; r < destImg.rows(); ++r)
                {
                    for(Index d = 0; d < destImg(r, c).size(); ++d)
                    {
                        // if magnitude is below low threshold, suppress this pixel
                        if(destImg(r, c)(d) < lowThreshold_)
                        {
                            destImg(r, c)(d) = 0;
                        }
                        // perform hyteresis if this is a weak pixel
                        else if(destImg(r, c)(d) < highThreshold_)
                        {
                            bool suppress = true;
                            for(Index dc = -1; dc < 2; ++dc)
                            {
                                Index c2 = border::handle(c + dc, 0, destImg.cols(), handling_);
                                for(Index dr = -1; dr < 2; ++dr)
                                {
                                    Index r2 = border::handle(r + dr, 0, destImg.rows(), handling_);
                                    if(destImg(r2, c2)(d) >= highThreshold_)
                                    {
                                        suppress = false;
                                        break;
                                    }
                                }
                            }

                            if(suppress)
                                destImg(r, c)(d) = 0;
                        }
                    }
                }
            }
        }
    };
}

#endif
