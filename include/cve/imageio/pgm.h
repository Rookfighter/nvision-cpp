/* pgm.h
 *
 * Author: Fabian Meyer
 * Created On: 03 Mar 2019
 */
#ifndef CVE_IMAGEIO_PGM_H_
#define CVE_IMAGEIO_PGM_H_

#include <string>
#include <fstream>
#include "cve/image.h"

namespace cve
{
    namespace pgm
    {
        template<typename Image>
        void load(const std::string &filename, Image &img)
        {
            std::ifstream is(filename);

            if(is.bad() || is.fail())
                throw std::runtime_error("cannot open " + filename);

            // find start signature "P5"
            while(!is.eof() && is.get() != 'P');

            if(is.eof() || is.get() != '5')
                throw std::runtime_error(filename + ": missing PGM start sequence");

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

            Index width = 0;
            while(!is.eof() && value >= 48 && value < 58)
            {
                width = 10 * width + value - 48;
                value = is.get();
            }

            // find next valid sequence
            while(!is.eof() && (value < 48 || value >= 58))
                value = is.get();

            Index height = 0;
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

            // find next line
            while(is.eof() && value != '\n')
                value = is.get();

            if(is.eof())
                throw std::runtime_error(filename + ": incomplete PGM file");

            if(width <= 0 || height <= 0)
                throw std::runtime_error(filename + ": invalid PGM image size");
            img.setZero(height, width);

            // read image data, data is stored in row major format
            for(Index r = 0; r < height; ++r)
            {
                for(Index c = 0; c < width; ++c)
                {
                    uint8_t val = is.get();
                    for(Index d = 0; d < img.depth(); ++d)
                        img(r, c, d) = val;
                }
            }
        }

        template<typename Image>
        void save(const std::string &filename, const Image &img)
        {
            std::ofstream os(filename);

            os << "P5\n"
                << img.cols() << ' '
                << img.rows() << '\n'
                << "255" << '\n';

            for(Index row = 0; row < img.rows(); ++row)
                for(Index col = 0; col < img.cols(); ++col)
                    os << static_cast<uint8_t>(img(row, col, 0));
        }
    }
}

#endif
