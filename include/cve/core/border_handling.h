/* border_handling.h
 *
 * Author: Fabian Meyer
 * Created On: 26 Apr 2019
 */

#ifndef CVE_BORDER_HANDLING_H_
#define CVE_BORDER_HANDLING_H_

#include <unsupported/Eigen/CXX11/Tensor>
#include "cve/core/matrix.h"
#include "cve/core/assert.h"

namespace cve
{
    /** Border handling functor, which returns a constant when indices are out
      * of bounds.
      * A row of a3x3 image would be treated by this functor as follows
      * (with value 0):
      *
      * 0 0 |3 1 2| 0 0 */
    template<typename Scalar>
    class BorderConstant
    {
    private:
        Scalar value_;
    public:
        BorderConstant()
            : BorderConstant(0)
        { }

        BorderConstant(const Scalar value)
            : value_(value)
        { }

        template<typename ScalarA>
        ScalarA operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const Index row,
            const Index col,
            const Index depth) const
        {
            if(row < 0 || row >= img.dimension(0)
                || col < 0 || col >= img.dimension(1))
                return static_cast<ScalarA>(value_);
            else
                return img(row, col, depth);

        }
    };

    /** Border handling functor, which reflects the image values when indices
      * are out of bounds
      * A row of a 3x3 image would be treated by this functor as follows:
      *
      * 2 1 |3 1 2| 1 3 */
    class BorderReflect
    {
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

    public:
        BorderReflect()
        { }

        template<typename ScalarA>
        ScalarA operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const Index row,
            const Index col,
            const Index depth) const
        {
            Index r = reflect(row, 0, img.dimension(0));
            Index c = reflect(col, 0, img.dimension(1));
            return img(r, c, depth);
        }
    };

    /** Border handling functor, which repeats the image values when indices
      * are out of bounds
      * A row of a 3x3 image would be treated by this functor as follows:
      *
      * 3 3 |3 1 2| 2 2 */
    class BorderRepeat
    {
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

    public:
        BorderRepeat()
        { }

        template<typename ScalarA>
        ScalarA operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const Index row,
            const Index col,
            const Index depth) const
        {
            Index r = repeat(row, 0, img.dimension(0));
            Index c = repeat(col, 0, img.dimension(1));
            return img(r, c, depth);
        }
    };
}

#endif
