/* color_generic_ops.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef NVISION_COLOR_GENERIC_OPS_H_
#define NVISION_COLOR_GENERIC_OPS_H_

#include "nvision/src/core/color_space.h"
#include "nvision/src/draw/color_type.h"

namespace nvision::color
{
    template<typename ColorSpace>
    inline auto red()   { return color::convert<RGBf, ColorSpace>(Color<RGBf>{1, 0, 0}); }

    template<typename ColorSpace>
    inline auto green() { return color::convert<RGBf, ColorSpace>(Color<RGBf>{0, 1, 0}); }

    template<typename ColorSpace>
    inline auto blue()  { return color::convert<RGBf, ColorSpace>(Color<RGBf>{0, 0, 1}); }

    template<typename ColorSpace>
    inline auto darkRed()   { return color::convert<RGBf, ColorSpace>(Color<RGBf>{0.545f, 0, 0}); }

    template<typename ColorSpace>
    inline auto darkGreen() { return color::convert<RGBf, ColorSpace>(Color<RGBf>{0, 0.392f, 0}); }

    template<typename ColorSpace>
    inline auto darkBlue()  { return color::convert<RGBf, ColorSpace>(Color<RGBf>{0, 0, 0.545f}); }

    // inline auto lightGreen() { return Array3(144, 238, 144); }
    // inline auto lightBlue()  { return Array3(173, 216, 230); }

    template<typename ColorSpace>
    inline auto magenta() { return color::convert<RGBf, ColorSpace>(Color<RGBf>{1, 0, 1}); }

    template<typename ColorSpace>
    inline auto yellow()  { return color::convert<RGBf, ColorSpace>(Color<RGBf>{1, 1, 0}); }

    template<typename ColorSpace>
    inline auto orange()  { return color::convert<RGBf, ColorSpace>(Color<RGBf>{1, 0.647f, 0}); }
    // static Array3 Pink() { return Array3(255, 192, 203); }
    // static Array3 Violet() { return Array3(183, 130, 237); }

    template<typename ColorSpace>
    inline auto black()     { return color::convert<RGBf, ColorSpace>(Color<RGBf>{0, 0, 0}); }

    template<typename ColorSpace>
    inline auto white()     { return color::convert<RGBf, ColorSpace>(Color<RGBf>{1, 1, 1}); }

    template<typename ColorSpace>
    inline auto grey()      { return color::convert<RGBf, ColorSpace>(Color<RGBf>{0.329f, 0.329f, 0.329f}); }

    template<typename ColorSpace>
    inline auto lightGrey() { return color::convert<RGBf, ColorSpace>(Color<RGBf>{0.706f, 0.706f, 0.706f}); }
}

#endif
