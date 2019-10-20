/* pgm.h
 *
 * Author: Fabian Meyer
 * Created On: 03 Mar 2019
 */
#ifndef CVE_IMAGEIO_PGM_H_
#define CVE_IMAGEIO_PGM_H_

#include <string>
#include <cctype>
#include <fstream>
#include <istream>
#include <ostream>
#include <unsupported/Eigen/CXX11/Tensor>

namespace cve
{
    namespace pgm
    {
        inline void __skip(std::istream &is, int &value)
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

        /** Load an image from a stream in PGM file format.
          * @param is input stream
          * @param img output image */
        template<typename Scalar>
        inline void load(std::istream &is,
            Eigen::Tensor<Scalar, 3> &img)
        {
            // find start signature "P5"
            while(!is.eof() && is.get() != 'P');

            if(is.eof() || is.get() != '5')
                throw std::runtime_error("missing PGM start sequence");

            int value = is.get();
            pgm::__skip(is, value);

            // parse width of image
            Index width = 0;
            while(!is.eof() && std::isdigit(value))
            {
                width = 10 * width + value - 48;
                value = is.get();
            }

            // find next valid sequence
            pgm::__skip(is, value);

            Index height = 0;
            while(!is.eof() && std::isdigit(value))
            {
                height = 10 * height + value - 48;
                value = is.get();
            }

            pgm::__skip(is, value);

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
                throw std::runtime_error("incomplete PGM file");

            if(width <= 0 || height <= 0)
                throw std::runtime_error("invalid PGM image size");
            img.resize(height, width, 1);
            img.setZero();

            // read image data, data is stored in row major format
            for(Index r = 0; r < height; ++r)
            {
                for(Index c = 0; c < width; ++c)
                {
                    uint8_t val = (is.get() * 255) / pixmax;
                    img(r, c, 0) = val;
                }
            }
        }

        /** Load an image from a file in PGM file format.
          * @param filename input file
          * @param img output image */
        template<typename Scalar>
        inline void load(const std::string &filename,
            Eigen::Tensor<Scalar, 3> &img)
        {
            std::ifstream is(filename);

            if(is.bad() || is.fail())
                throw std::runtime_error("cannot open " + filename);

            pgm::load(is, img);
        }

        /** Save an image to a stream in PGM file format.
          * @param os ouput stream
          * @param img input image */
        template<typename Scalar>
        inline void save(std::ostream &os,
            const Eigen::Tensor<Scalar, 3> &img)
        {
            os << "P5\n"
                << img.dimension(1) << ' '
                << img.dimension(0) << '\n'
                << "255" << '\n';

            for(Index r = 0; r < img.dimension(0); ++r)
                for(Index c = 0; c < img.dimension(1); ++c)
                    os << static_cast<uint8_t>(img(r, c, 0));
        }

        /** Save an image to a file in PGM file format.
          * @param filename ouput file
          * @param img input image */
        template<typename Scalar>
        inline void save(const std::string &filename,
            const Eigen::Tensor<Scalar, 3> &img)
        {
            std::ofstream os(filename);
            pgm::save(os, img);
        }
    }
}

#endif
