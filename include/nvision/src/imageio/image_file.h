/* image_file.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef NVISION_IMAGE_FILE_H_
#define NVISION_IMAGE_FILE_H_

#include <fstream>

#include "nvision/src/imageio/image_file_format.h"
#include "nvision/src/imageio/image_file_format_ppm.h"
#include "nvision/src/imageio/image_file_format_pgm.h"
#include "nvision/src/imageio/image_file_format_bmp.h"
#include "nvision/src/imageio/image_file_format_png.h"

namespace nvision
{
    inline std::string extension(const std::string &path)
    {
        size_t pos = path.find_last_of('.');
        if(pos == std::string::npos)
            throw std::runtime_error("filename \"" + path + "\" has no extension");

        return path.substr(pos+1);
    }

    template<typename Derived, typename FileFormat>
    inline void imread(std::istream &stream, ImageBase<Derived> &img, const FileFormat &)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
        const ImageReader<FileFormat> reader;
        reader(stream, img);
    }

    template<typename Derived, typename FileFormat>
    inline void imload(const std::string &path, ImageBase<Derived> &img, const FileFormat &format)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
        std::ifstream stream(path, std::ifstream::binary);
        imread(stream, img, format);
    }

    template<typename Derived>
    inline void imload(const std::string &path, ImageBase<Derived> &img)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
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

    template<typename Derived, typename FileFormat>
    inline void imwrite(std::ostream &stream, const ImageBase<Derived> &img, const FileFormat &format)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
        const ImageWriter<FileFormat> writer;
        writer(stream, img, format);
    }

    template<typename Derived, typename FileFormat>
    inline void imsave(const std::string &path, const ImageBase<Derived> &img, const FileFormat &format)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
        std::ofstream stream(path, std::ofstream::binary);
        imwrite(stream, img, format);
    }

    template<typename Derived>
    inline void imsave(const std::string &path, const ImageBase<Derived> &img)
    {
        static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
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
