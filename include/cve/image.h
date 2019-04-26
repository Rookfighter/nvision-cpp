/* image.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_IMAGE_H_
#define CVE_IMAGE_H_

#include <Eigen/Geometry>

namespace cve
{
    typedef long int Index;

    namespace border
    {
        inline Index reflect(const Index idx, const Index min, const Index max)
        {
            if(idx < min)
                return min + std::abs(idx);
            else if(idx >= max)
                return 2 * max - std::abs(idx) - 1;
            else
                return idx;
        }

        inline Index repeat(const Index idx, const Index min, const Index max)
        {
            if(idx < min)
                return min;
            else if (idx >= max)
                return max - 1;
            else
                return idx;
        }
    }

    template<typename Scalar,
        int Depth,
        int Height = Eigen::Dynamic,
        int Width = Eigen::Dynamic>
    struct Image
    {
        static_assert(Depth > 0, "Image depth has to be greater than zero");

        typedef Eigen::Array<Eigen::Array<Scalar, Depth, 1>, Height, Width> Array;
    };

    typedef Image<uint8_t, 1>::Array ImageGray;
    typedef Image<uint8_t, 3>::Array ImageRGB;
    typedef Image<uint8_t, 4>::Array ImageRGBA;
}

#endif
