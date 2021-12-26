/* image_file.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_IMAGE_FILE_H_
#define CVE_IMAGE_FILE_H_

#include <fstream>

#include "cve/src/imageio/image_file_format.h"
#include "cve/src/imageio/image_file_format_ppm.h"
#include "cve/src/imageio/image_file_format_pgm.h"
#include "cve/src/imageio/image_file_format_bmp.h"
#include "cve/src/imageio/image_file_format_png.h"

namespace cve
{
    std::string extension(const std::string &path)
    {
        size_t pos = path.find_last_of('.');
        if(pos == std::string::npos)
            throw std::runtime_error("filename \"" + path + "\" has no extension");

        return path.substr(pos+1);
    }

    template<typename ColorSpace, typename FileFormat>
    void imread(std::istream &stream, Image<ColorSpace> &img, const FileFormat &)
    {
        const ImageReader<FileFormat> reader;
        reader(stream, img);
    }

    template<typename ColorSpace, typename FileFormat>
    void imload(const std::string &path, Image<ColorSpace> &img, const FileFormat &format)
    {
        std::ifstream stream(path, std::ifstream::binary);
        imread(stream, img, format);
    }

    template<typename ColorSpace>
    void imload(const std::string &path, Image<ColorSpace> &img)
    {
        const auto ext = extension(path);

        if(ext == "ppm")
            imload(path, img, PPM());
        else if(ext == "pgm")
            imload(path, img, PGM());
        else if(ext == "bmp")
            imload(path, img, BMP());
        else if(ext == "png")
            imload(path, img, PNG());
        else
            throw std::runtime_error("unkown file type");
    }

    template<typename ColorSpace, typename FileFormat>
    void imwrite(std::ostream &stream, const Image<ColorSpace> &img, const FileFormat &format)
    {
        const ImageWriter<FileFormat> writer;
        writer(stream, img, format);
    }

    template<typename ColorSpace, typename FileFormat>
    void imsave(const std::string &path, const Image<ColorSpace> &img, const FileFormat &format)
    {
        std::ofstream stream(path, std::ofstream::binary);
        imwrite(stream, img, format);
    }

    template<typename ColorSpace>
    void imsave(const std::string &path, const Image<ColorSpace> &img)
    {
        const auto ext = extension(path);

        if(ext == "ppm")
            imsave(path, img, PPM());
        else if(ext == "pgm")
            imsave(path, img, PGM());
        else if(ext == "bmp")
            imsave(path, img, BMP());
        else if(ext == "png")
            imsave(path, img, PNG());
        else
            throw std::runtime_error("unkown file type");
    }
}

#endif