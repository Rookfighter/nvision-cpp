/* ppm.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */
#ifndef CVE_IMAGEIO_PPM_H_
#define CVE_IMAGEIO_PPM_H_

#include <string>
#include <fstream>
#include "cve/image.h"

namespace cve
{
namespace ppm
{
    void load(const std::string &filename, ImageRGB &img)
    {
        std::ifstream is(filename);

        if(is.bad() || is.fail())
            throw std::runtime_error("cannot open " + filename);

        // find start signature "P6"
        while(!is.eof() && is.get() != 'P');

        if(is.eof() || is.get() != '6')
            throw std::runtime_error(filename + ": missing PPM start sequence");

        int value = is.get();
        while(!is.eof() && value != ' ' && value != '\n')
            value = is.get();

        // check if line is a comment
        value = is.get();
        while(!is.eof() && value == '#')
        {
            // search for end of line
            while(is.get() != '\n');
            // update value to first char of line
            value = is.get();
        }

        // trim all empty lines
        while(!is.eof() && value == '\n')
            value = is.get();

        ImageGray::Index width = 0;
        while(!is.eof() && value >= 48 && value < 58)
        {
            width = 10 * width + value - 48;
            value = is.get();
        }

        // find next valid sequence
        while(!is.eof() && (value < 48 || value >= 58))
            value = is.get();

        ImageGray::Index height = 0;
        while(!is.eof() && value >= 48 && value < 58)
        {
            height = 10 * height + value - 48;
            value = is.get();
        }

        while(!is.eof() && value != ' ' && value != '\n')
            value = is.get();

        value = is.get();
        while(!is.eof() && (value < 48 || value >= 58))
            value = is.get();

        while(!is.eof() && (value >= 48 || value < 58))
            value = is.get();

        // find next line
        while(is.eof() && value != '\n')
            value = is.get();

        if(is.eof())
            throw std::runtime_error(filename + ": incomplete PPM file");

        if(width <= 0 || height <= 0)
            throw std::runtime_error(filename + ": invalid PPM image size");
        img.resize(height, width);

        // read image data, data is stored in row major format
        for(ImageRGB::Index row = 0; row < height; ++row)
        {
            for(ImageRGB::Index col = 0; col < width; ++col)
            {
                img(row, col) << is.get(), is.get(), is.get();
            }
        }
    }

    void save(const std::string &filename, const ImageRGB &img)
    {
        std::ofstream os(filename);

        os << "P6\n"
            << img.cols() << ' '
            << img.rows() << '\n'
            << "255" << '\n';

        for(ImageRGB::Index row = 0; row < img.rows(); ++row)
        {
            for(ImageRGB::Index col = 0; col < img.cols(); ++col)
            {
                os << img(row, col)(0), img(row, col)(1), img(row, col)(2);
            }
        }
    }
}
}

#endif
