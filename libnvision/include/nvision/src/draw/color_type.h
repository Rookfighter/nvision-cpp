/* color_type.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef NVISION_COLOR_TYPE_H_
#define NVISION_COLOR_TYPE_H_

#include <array>
#include "nvision/src/core/types.h"

namespace nvision
{
    /** Generic pixel class which provides general purpose arithmetic operations
      * and color space safety. */
    template<typename _ColorSpace>
    class Color
    {
    public:
        using ColorSpace = _ColorSpace;
        using ValueType = typename ColorSpace::ValueType;
        static constexpr Index Dimension = ColorSpace::Dimension;

        Color() = default;

        /** Constructs a color where all channels are set to the given value. */
        explicit Color(const ValueType value)
        {
            for(Index i = 0; i < Dimension; ++i)
                _array[i] = value;
        }

        /** Constructs a color with three channels and their respective values. */
        explicit Color(const ValueType value0, const ValueType value1, const ValueType value2)
            : _array{value0, value1, value2}
        {
            static_assert(Dimension == 3, "Color must have dimension 3");
        }

        /** Constructs a color with four channels and their respective values. */
        explicit Color(const ValueType value0, const ValueType value1, const ValueType value2, const ValueType value3)
            : _array{value0, value1, value2, value3}
        {
            static_assert(Dimension == 4, "Color must have dimension 4");
        }

        /** Returns the number of channels of this pixel. */
        Index size() const
        {
            return Dimension;
        }

        /** Converts this pixel to the given color space..
          * @return the converted pixel */
        template<typename TargetColorSpace>
        Color<TargetColorSpace> convert() const;

        /** Provides access to the individual channels of this pixel. */
        decltype(auto) operator[](const Index idx) const
        {
            return _array[idx];
        }

        bool operator==(const Color<ColorSpace> &rhs) const
        {
            for(Index i = 0; i < Dimension; ++i)
            {
                if(_array[i] != rhs._array[i])
                    return false;
            }

            return true;
        }

        bool operator!=(const Color<ColorSpace> &rhs) const
        {
            return !this->operator==(rhs);
        }

    private:
        std::array<ValueType, Dimension> _array = {};
    };
}

#endif
