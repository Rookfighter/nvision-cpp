/* color_converter.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef CVE_COLOR_CONVERTER_H_
#define CVE_COLOR_CONVERTER_H_

#include "cve/image.h"

namespace cve
{
    class ColorConverter
    {
    public:
        ColorConverter()
        { }

        template<typename HSV, typename RGB>
        Eigen::Array<RGB, 3, 1> hsv2rgb(const Eigen::Array<HSV, 3, 1> &hsv) const
        {
            Eigen::Array<RGB, 3, 1> rgb;
            HSV h = hsv(0);
            HSV s = hsv(1);
            HSV v = hsv(2);
            if(s <= 0)
            {
                rgb << v, v, v;
                return rgb;
            }

            if(h >= 360)
                h = 0;
            HSV region = h / 60.0;
            Index idx = static_cast<Index>(region);
            HSV rest = region - idx;
            HSV p = v * (1.0 - s);
            HSV q = v * (1.0 - s * rest);
            HSV t = v * (1.0 - s * (1.0 - rest));
            switch(idx)
            {
            case 0:
                rgb << v * 255, t * 255, p * 255;
                break;
            case 1:
                rgb << q * 255, v * 255, p * 255;
                break;
            case 2:
                rgb << p * 255, v * 255, t * 255;
                break;
            case 3:
                rgb << p * 255, q * 255, v * 255;
                break;
            case 4:
                rgb << t * 255, p * 255, v * 255;
                break;
            case 5:
            default:
                rgb << v * 255, p * 255, q * 255;
            }
            return rgb;
        }

        template<typename RGB, typename HSV>
        Eigen::Array<HSV, 3, 1> rgb2hsv(const Eigen::Array<RGB, 3, 1> &rgb) const
        {
            Eigen::Array<HSV, 3, 1> hsv;
            HSV r = rgb(0);
            HSV g = rgb(1);
            HSV b = rgb(2);
            HSV minVal = rgb.minCoeff();
            HSV maxVal = rgb.maxCoeff();
            HSV delta = maxVal - minVal;

            hsv(2) = maxVal;
            if(maxVal > 0)
                hsv(1) = delta / maxVal;
            else
            {
                hsv(0) = 0;
                hsv(1) = 0;
                return hsv;
            }

            if(r >= maxVal)
                hsv(0) = (g - b) / delta;
            else if(g >= maxVal)
                hsv(0) = 2 + (b - r) / delta;
            else
                hsv(0) = 4 + (r - g) / delta;
            hsv(0) *= 60;

            if(hsv(0) < 0)
                hsv(0) += 360;

            return hsv;
        }
    };
}

#endif
