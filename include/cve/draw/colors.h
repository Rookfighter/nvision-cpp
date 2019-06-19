/* colors.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_COLORS_H_
#define CVE_COLORS_H_

#include "cve/image.h"

namespace cve
{

    template<typename Scalar>
    class Color
    {
    public:
        typedef Eigen::Array<Scalar, 3, 1> Array3;

        static Array3 Red() { return Array3(255, 0, 0); }
        static Array3 Green() { return Array3(0, 255, 0); }
        static Array3 Blue() { return Array3(0, 0, 255); }
        static Array3 Magenta() { return Array3(255, 0, 255); }
        static Array3 Yellow() { return Array3(255, 255, 0); }
        static Array3 Black() { return Array3(0, 0, 0); }
        static Array3 White() { return Array3(255, 255, 255); }
        static Array3 Grey() { return Array3(84, 84, 84); }
        static Array3 LightGrey() { return Array3(180, 180, 180); }
    };

    typedef Color<uint8_t> Color8;
    typedef Color<uint16_t> Color16;
    typedef Color<uint32_t> Color32;
    typedef Color<float> Colorf;
    typedef Color<double> Colord;
}

#endif
