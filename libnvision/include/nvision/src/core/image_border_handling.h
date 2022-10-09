/* image_border_handling.h
 *
 * Author: Fabian Meyer
 * Created On: 26 Apr 2019
 */

#ifndef NVISION_IMAGE_BORDER_HANDLING_H_
#define NVISION_IMAGE_BORDER_HANDLING_H_

#include "nvision/src/core/image_type.h"

namespace nvision
{
    /** Border handling functor, which returns a constant when indices are out
      * of bounds.
      * A row of a3x3 image would be treated by this functor as follows
      * (with value 0):
      *
      * 0 0 |3 1 2| 0 0 */
    template<typename _ColorSpace>
    class BorderConstant
    {
    public:
        static_assert(IsColorSpace<_ColorSpace>::value, "template parameter must be a color space");
        using ColorSpace = _ColorSpace;

        BorderConstant()
            : _value(0)
        { }

        BorderConstant(const Pixel<ColorSpace> &value)
            : _value(value)
        { }

        template<typename Derived>
        const Pixel<ColorSpace> &operator()(const ImageBase<Derived> &img,
                                            const Index row,
                                            const Index col) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "input is not a valid image type");

            if(row < 0 || row >= img.rows() || col < 0 || col >= img.cols())
                return _value;
            else
                return img(row, col);

        }
    private:
        const Pixel<ColorSpace> _value;
    };

    /** Border handling functor, which reflects the image values when indices
      * are out of bounds.
      * A row of a 3x3 image would be treated by this functor as follows:
      *
      * 2 1 |3 1 2| 1 3 */
    class BorderReflect
    {
    public:
        template<typename Derived>
        const typename ImageBase<Derived>::Scalar &operator()(const ImageBase<Derived> &img,
                                            const Index row,
                                            const Index col) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "input is not a valid image type");

            const auto r = reflect(row, 0, img.rows());
            const auto c = reflect(col, 0, img.cols());
            return img(r, c);
        }

    private:
        Index reflect(const Index idx, const Index minval, const Index maxval) const
        {
            if(idx < minval)
                return minval + std::abs(minval - idx);
            else if(idx >= maxval)
                return maxval - 2 - std::abs(idx - maxval);
            else
                return idx;
        }
    };

    /** Border handling functor, which repeats the image values when indices
      * are out of bounds.
      * A row of a 3x3 image would be treated by this functor as follows:
      *
      * 3 3 |3 1 2| 2 2 */
    class BorderRepeat
    {
    public:
        template<typename Derived>
        const typename ImageBase<Derived>::Scalar &operator()(const ImageBase<Derived> &img,
                           const Index row,
                           const Index col) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "input is not a valid image type");

            const auto r = repeat(row, 0, img.rows());
            const auto c = repeat(col, 0, img.cols());
            return img(r, c);
        }
    private:
        Index repeat(const Index idx, const Index minval, const Index maxval) const
        {
            if(idx < minval)
                return minval;
            else if (idx >= maxval)
                return maxval - 1;
            else
                return idx;
        }
    };
}

#endif
