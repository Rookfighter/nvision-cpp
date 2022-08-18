/* color_space.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Dec 2019
 */

#ifndef NVISION_COLOR_SPACE_H_
#define NVISION_COLOR_SPACE_H_

#include "nvision/src/core/color_space_rgb.h"
#include "nvision/src/core/color_space_rgba.h"
#include "nvision/src/core/color_space_bgr.h"
#include "nvision/src/core/color_space_bgra.h"
#include "nvision/src/core/color_space_gray.h"
#include "nvision/src/core/color_space_hsv.h"
#include "nvision/src/core/color_space_ycbcr.h"

namespace nvision
{
    template<typename ColorSpace>
    template<typename TargetColorSpace>
    inline Pixel<TargetColorSpace> Pixel<ColorSpace>::convert() const
    {
        return pixel::convert<ColorSpace, TargetColorSpace>(*this);
    }
}

#endif
