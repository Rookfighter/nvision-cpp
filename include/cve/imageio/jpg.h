/* jpg.h
 *
 * Author: Fabian Meyer
 * Created On: 20 Jun 2019
 */

#ifndef CVE_JPG_H_
#define CVE_JPG_H_

#include <CImg.h>

namespace cve
{
    namespace jpg
    {
        template<typename Scalar>
        void load(const std::string &filename, Eigen::Tensor<Scalar, 3> &img)
        {
            cimg_library::CImg<unsigned char> tmp(filename.c_str());

            img.resize(tmp.height(), tmp.width(), tmp.depth());

            for(Index d = 0; d < img.dimension(2); ++d)
                for(Index c = 0; c < img.dimension(1); ++c)
                    for(Index r = 0; r < img.dimension(0); ++r)
                        img(r, c, d) = static_cast<Scalar>(tmp(c, r, d));
        }
    }
}

#endif
