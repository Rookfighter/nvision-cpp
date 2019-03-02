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
