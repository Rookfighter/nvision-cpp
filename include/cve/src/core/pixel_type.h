/* pixel_type.h
 *
 * Author: Fabian Meyer
 * Created On: 23 Dec 2021
 */

#ifndef CVE_PIXEL_TYPE_H_
#define CVE_PIXEL_TYPE_H_

#include "cve/src/core/types.h"

namespace cve
{
    /** Generic pixel class which provides general purpose arithmetic operations
      * and color space safety. */
    template<typename _ColorSpace>
    class Pixel
    {
    public:
        using ColorSpace = _ColorSpace;
        using ValueType = typename ColorSpace::ValueType;
        constexpr static Index Dimension = ColorSpace::Dimension;

        Pixel() = default;

        /** Constructs a pixel where all channels are set to the given value. */
        explicit Pixel(const ValueType value)
        {
            setConstant(value);
        }

        /** Constructs a pixel with three channels and their respective values. */
        explicit Pixel(const ValueType value0, const ValueType value1, const ValueType value2)
            : _array(value0, value1, value2)
        {
            static_assert(Dimension == 3, "Pixel must have dimension 3");
        }

        /** Constructs a pixel with four channels and their respective values. */
        explicit Pixel(const ValueType value0, const ValueType value1, const ValueType value2, const ValueType value3)
            : _array(value0, value1, value2, value3)
        {
            static_assert(Dimension == 4, "Pixel must have dimension 4");
        }

        /** Sets all channels to zero. */
        void setZero()
        {
            _array.setZero();
        }

        /** Sets all channels to one. */
        void setOnes()
        {
            _array.setOnes();
        }

        /** Sets all channels to given value. */
        void setConstant(const ValueType value)
        {
            _array.setConstant(value);
        }

        /** Returns the number of channels of this pixel. */
        Index size() const
        {
            return Dimension;
        }

        /** Converts this pixel to the given color space..
          * @return the converted pixel */
        template<typename TargetColorSpace>
        Pixel<TargetColorSpace> convert() const;

        /** Provides access to the individual channels of this pixel. */
        decltype(auto) operator[](const Index idx)
        {
            return _array(idx);
        }

        /** Provides access to the individual channels of this pixel. */
        decltype(auto) operator[](const Index idx) const
        {
            return _array(idx);
        }

        Pixel<ColorSpace>& operator*=(const Pixel<ColorSpace> &rhs)
        {
            _array *= rhs._array;
            return *this;
        }

        Pixel<ColorSpace>& operator/=(const Pixel<ColorSpace> &rhs)
        {
            _array /= rhs._array;
            return *this;
        }

        Pixel<ColorSpace>& operator-=(const Pixel<ColorSpace> &rhs)
        {
            _array -= rhs._array;
            return *this;
        }

        Pixel<ColorSpace>& operator+=(const Pixel<ColorSpace> &rhs)
        {
            _array += rhs._array;
            return *this;
        }

        Pixel<ColorSpace> operator-() const
        {
            Pixel<ColorSpace> result;
            for(Index i = 0; i < result.size(); ++i)
                result[i] = -_array(i);
            return result;
        }

        template<typename Scalar2>
        Pixel<ColorSpace>& operator*=(const Scalar2 &rhs)
        {
            for(Index i = 0; i < _array.size(); ++i)
                _array[i] = static_cast<ValueType>(_array[i] * rhs);
            return *this;
        }

        template<typename Scalar2>
        Pixel<ColorSpace>& operator/=(const Scalar2 &rhs)
        {
            for(Index i = 0; i < _array.size(); ++i)
                _array[i] = static_cast<ValueType>(_array[i] / rhs);
            return *this;
        }

        template<typename Scalar2>
        Pixel<ColorSpace>& operator-=(const Scalar2 &rhs)
        {
            for(Index i = 0; i < _array.size(); ++i)
                _array[i] = static_cast<ValueType>(_array[i] - rhs);
            return *this;
        }

        template<typename Scalar2>
        Pixel<ColorSpace>& operator+=(const Scalar2 &rhs)
        {
            for(Index i = 0; i < _array.size(); ++i)
                _array[i] = static_cast<ValueType>(_array[i] + rhs);
            return *this;
        }

        bool operator==(const Pixel<ColorSpace> &rhs) const
        {
            for(Index i = 0; i < Dimension; ++i)
            {
                if(_array[i] != rhs._array[i])
                    return false;
            }

            return true;
        }

        bool operator!=(const Pixel<ColorSpace> &rhs) const
        {
            return !this->operator==(rhs);
        }

    private:
        Eigen::Array<ValueType, Dimension, 1> _array = {};
    };

    template<typename ColorSpace>
    inline const Pixel<ColorSpace> operator*(const Pixel<ColorSpace> &lhs, const Pixel<ColorSpace> &rhs)
    {
        auto result = Pixel<ColorSpace>{lhs};
        result *= rhs;
        return result;
    }

    template<typename ColorSpace>
    inline const Pixel<ColorSpace> operator/(const Pixel<ColorSpace> &lhs, const Pixel<ColorSpace> &rhs)
    {
        auto result = Pixel<ColorSpace>{lhs};
        result /= rhs;
        return result;
    }

    template<typename ColorSpace>
    inline const Pixel<ColorSpace> operator-(const Pixel<ColorSpace> &lhs, const Pixel<ColorSpace> &rhs)
    {
        auto result = Pixel<ColorSpace>{lhs};
        result -= rhs;
        return result;
    }

    template<typename ColorSpace>
    inline const Pixel<ColorSpace> operator+(const Pixel<ColorSpace> &lhs, const Pixel<ColorSpace> &rhs)
    {
        auto result = Pixel<ColorSpace>{lhs};
        result += rhs;
        return result;
    }

    template<typename ColorSpace, typename Scalar2>
    inline const Pixel<ColorSpace> operator*(const Pixel<ColorSpace> &lhs, const Scalar2 &rhs)
    {
        auto result = Pixel<ColorSpace>{lhs};
        result *= rhs;
        return result;
    }

    template<typename ColorSpace, typename Scalar2>
    inline const Pixel<ColorSpace> operator/(const Pixel<ColorSpace> &lhs, const Scalar2 &rhs)
    {
        auto result = Pixel<ColorSpace>{lhs};
        result /= rhs;
        return result;
    }

    template<typename ColorSpace, typename Scalar2>
    inline const Pixel<ColorSpace> operator-(const Pixel<ColorSpace> &lhs, const Scalar2 &rhs)
    {
        auto result = Pixel<ColorSpace>{lhs};
        result -= rhs;
        return result;
    }

    template<typename ColorSpace, typename Scalar2>
    inline const Pixel<ColorSpace> operator+(const Pixel<ColorSpace> &lhs, const Scalar2 &rhs)
    {
        auto result = Pixel<ColorSpace>{lhs};
        result += rhs;
        return result;
    }

    template<typename ColorSpace, typename Scalar2>
    inline const Pixel<ColorSpace> operator*(const Scalar2 &lhs, const Pixel<ColorSpace> &rhs)
    {
        auto result = Pixel<ColorSpace>{rhs};
        for(Index i = 0; i < result.size(); ++i)
            result[i] = static_cast<typename ColorSpace::ValueType>(lhs * result[i]);
        return result;
    }

    template<typename ColorSpace, typename Scalar2>
    inline const Pixel<ColorSpace> operator/(const Scalar2 &lhs, const Pixel<ColorSpace> &rhs)
    {
        auto result = Pixel<ColorSpace>{rhs};
        for(Index i = 0; i < result.size(); ++i)
            result[i] = static_cast<typename ColorSpace::ValueType>(lhs / result[i]);
        return result;
    }

    template<typename ColorSpace, typename Scalar2>
    inline const Pixel<ColorSpace> operator-(const Scalar2 &lhs, const Pixel<ColorSpace> &rhs)
    {
        auto result = Pixel<ColorSpace>{rhs};
        for(Index i = 0; i < result.size(); ++i)
            result[i] = static_cast<typename ColorSpace::ValueType>(lhs - result[i]);
        return result;
    }

    template<typename ColorSpace, typename Scalar2>
    inline const Pixel<ColorSpace> operator+(const Scalar2 &lhs, const Pixel<ColorSpace> &rhs)
    {
        auto result = Pixel<ColorSpace>{rhs};
        for(Index i = 0; i < result.size(); ++i)
            result[i] = static_cast<typename ColorSpace::ValueType>(lhs + result[i]);
        return result;
    }

    template<typename ColorSpace>
    std::ostream &operator<<(std::ostream &lhs, const Pixel<ColorSpace> &rhs)
    {
        auto writeChannel = [&lhs](auto value)
        {
            if(std::is_integral<typename ColorSpace::ValueType>::value)
                lhs << static_cast<int64>(value);
            else
                lhs << value;
        };

        lhs << '(';
        writeChannel(rhs[0]);
        for(Index i = 1; i < rhs.size(); ++i)
        {
            lhs << ',';
            writeChannel(rhs[i]);
        }
        lhs << ')';
        return lhs;
    }
}

#endif