/* imageio.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_IMAGEIO_H_
#define CVE_IMAGEIO_H_

#include "cve/imageio/ppm.h"
#include "cve/imageio/pgm.h"
#include "cve/imageio/png.h"
// #include "cve/imageio/jpg.h"

namespace cve
{
    std::string extension(const std::string &filename)
    {
        size_t pos = filename.find_last_of('.');
        if(pos == std::string::npos)
            throw std::runtime_error("filename \"" + filename + "\" has no extension");

        return filename.substr(pos+1);
    }

    template<typename Scalar>
    void imload(const std::string &filename, Eigen::Tensor<Scalar, 3> &img)
    {
        std::string ext = extension(filename);

        if(ext == "pgm")
            pgm::load(filename, img);
        else if(ext == "ppm")
            ppm::load(filename, img);
        else if(ext == "png")
            png::load(filename, img);
        // else if(ext == "jpg" || ext == "jpeg")
        //     jpg::load(filename, img);
        else
            throw std::runtime_error("imload: unknown file extension \"" + ext + "\"");
    }

    template<typename Scalar>
    void imsave(const std::string &filename, const Eigen::Tensor<Scalar, 3> &img)
    {
        std::string ext = extension(filename);

        if(ext == "pgm")
            pgm::save(filename, img);
        else if(ext == "ppm")
            ppm::save(filename, img);
        else
            throw std::runtime_error("imsave: unknown file extension \"" + ext + "\"");
    }
}

#endif
