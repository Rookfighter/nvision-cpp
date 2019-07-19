/* colors.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_COLORS_H_
#define CVE_COLORS_H_

#include <vector>
#include "cve/core/image.h"

namespace cve
{

    template<typename Scalar>
    struct Color
    {
        typedef Eigen::Array<Scalar, 3, 1> Array3;

        static Array3 Red() { return Array3(255, 0, 0); }
        static Array3 Green() { return Array3(0, 255, 0); }
        static Array3 Blue() { return Array3(0, 0, 255); }

        static Array3 DarkRed() { return Array3(139, 0, 0); }
        static Array3 DarkGreen() { return Array3(0, 100, 0); }
        static Array3 DarkBlue() { return Array3(0, 0, 139); }

        static Array3 LightGreen() { return Array3(144, 238, 144); }
        static Array3 LightBlue() { return Array3(173, 216, 230); }

        static Array3 Magenta() { return Array3(255, 0, 255); }
        static Array3 Yellow() { return Array3(255, 255, 0); }
        static Array3 Orange() { return Array3(255, 165, 0); }
        static Array3 Pink() { return Array3(255, 192, 203); }
        static Array3 Violet() { return Array3(183, 130, 237); }

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

    template<typename Scalar>
    class ColorSeries
    {
    public:
        typedef Eigen::Array<Scalar, 3, 1> Array3;
    private:
        std::vector<Array3> colors_;
        Index currIdx_;
    public:
        ColorSeries()
            : colors_(), currIdx_(0)
        {
            colors_ = {Color<Scalar>::Red(),
                Color<Scalar>::Green(),
                Color<Scalar>::Blue(),
                Color<Scalar>::Magenta(),
                Color<Scalar>::Yellow(),
                Color<Scalar>::Violet(),
                Color<Scalar>::Pink(),
                Color<Scalar>::Orange(),
                Color<Scalar>::LightGreen(),
                Color<Scalar>::LightBlue(),
                Color<Scalar>::DarkRed()};
        }

        ColorSeries(const std::vector<Array3> &colors)
            : colors_(colors), currIdx_(0)
        {

        }

        Index size() const
        {
            return static_cast<Index>(colors_.size());
        }

        const Array3 &next()
        {
            Index old = currIdx_;
            ++currIdx_;
            currIdx_ %= size();
            return at(old);
        }

        const Array3 &at(const Index idx) const
        {
            return colors_[idx];
        }

        const Array3 &operator[](const Index idx) const
        {
            return at(idx);
        }
    };
}

#endif
