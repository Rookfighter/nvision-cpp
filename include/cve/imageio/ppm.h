/* ppm.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */
#ifndef CVE_IMAGEIO_PPM_H_
#define CVE_IMAGEIO_PPM_H_

#include <string>
#include <fstream>

namespace cve
{
    namespace ppm
    {
        inline void _skip(std::ifstream &is, int &value)
        {
            bool done = false;
            while(!done)
            {
                done = true;
                // trim all white spaces
                while(!is.eof() && std::isspace(value))
                    value = is.get();
                // ignore all comment lines
                while(!is.eof() && value == '#')
                {
                    // search for end of line
                    while(is.get() != '\n');
                    // update value to first char of line
                    value = is.get();
                    done = false;
                }
            }
        }

        template<typename Image>
        void load(const std::string &filename, Image &img)
        {
            static_assert(Image::Depth >= 3, "Image must have a depth of 3 at least");

            std::ifstream is(filename);

            if(is.bad() || is.fail())
                throw std::runtime_error("cannot open " + filename);

            // find start signature "P6"
            while(!is.eof() && is.get() != 'P');

            if(is.eof() || is.get() != '6')
                throw std::runtime_error(filename + ": missing PPM start sequence");

            int value = is.get();
            _skip(is, value);

            Index width = 0;
            while(!is.eof() && std::isdigit(value))
            {
                width = 10 * width + value - 48;
                value = is.get();
            }

            _skip(is, value);

            Index height = 0;
            while(!is.eof() && std::isdigit(value))
            {
                height = 10 * height + value - 48;
                value = is.get();
            }

            _skip(is, value);

            int pixmax = 0;
            while(!is.eof() && std::isdigit(value))
            {
                pixmax = 10 * pixmax + value - 48;
                value = is.get();
            }

            // find next line
            while(!is.eof() && value != '\n')
                value = is.get();

            if(is.eof())
                throw std::runtime_error(filename + ": incomplete PPM file");

            if(width <= 0 || height <= 0)
                throw std::runtime_error(filename + ": invalid PPM image size");
            img.setZero(height, width);

            // read image data, data is stored in row major format
            for(Index r = 0; r < height; ++r)
            {
                for(Index c = 0; c < width; ++c)
                {
                    uint8_t red = (is.get() * 255) / pixmax;
                    uint8_t green = (is.get() * 255) / pixmax;
                    uint8_t blue = (is.get() * 255) / pixmax;
                    img(r, c, 0) = red;
                    img(r, c, 1) = green;
                    img(r, c, 2) = blue;
                }
            }
        }

        template<typename Image>
        void save(const std::string &filename, const Image &img)
        {
            static_assert(Image::Depth >= 3, "Image must have a depth of 3 at least");
            std::ofstream os(filename);

            os << "P6\n"
                << img.cols() << ' '
                << img.rows() << '\n'
                << "255" << '\n';

            for(Index r = 0; r < img.rows(); ++r)
            {
                for(Index c = 0; c < img.cols(); ++c)
                {
                    os << static_cast<uint8_t>(img(row, col, 0)),
                        static_cast<uint8_t>(img(row, col, 1)),
                        static_cast<uint8_t>(img(row, col, 2));
                }
            }
        }
    }
}

#endif
