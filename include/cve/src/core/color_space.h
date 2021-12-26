/* color_space.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Dec 2019
 */

#ifndef CVE_COLOR_SPACE_H_
#define CVE_COLOR_SPACE_H_

#include "cve/src/core/color_space_rgb.h"
#include "cve/src/core/color_space_rgba.h"
#include "cve/src/core/color_space_bgr.h"
#include "cve/src/core/color_space_bgra.h"
#include "cve/src/core/color_space_gray.h"
#include "cve/src/core/color_space_hsv.h"
#include "cve/src/core/color_space_ycbcr.h"

namespace cve
{
    template<typename ColorSpace>
    template<typename TargetColorSpace>
    inline Pixel<TargetColorSpace> Pixel<ColorSpace>::convert() const
    {
        return pixel::convert<ColorSpace, TargetColorSpace>(*this);
    }
}

#endif