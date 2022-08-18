/* image_file_format_ppm.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef NVISION_IMAGE_FILE_FORMAT_PPM_H_
#define NVISION_IMAGE_FILE_FORMAT_PPM_H_

#include "nvision/src/core/image.h"
#include "nvision/src/imageio/image_file_format.h"

namespace nvision
{
    struct PPM
    { };

    /** Reads an image from a stream in PPM file format. */
    template<>
    class ImageReader<PPM>
    {
    public:
        template<typename ColorSpace>
        void operator()(std::istream &stream, Image<ColorSpace> &img) const
        {
            // find start signature "P6"
            while(stream.good() && stream.get() != 'P')
            { }

            if(!stream.good() || stream.get() != '6')
                throw std::runtime_error("missing PPM start sequence");

            skipComments(stream);
            Index width = readInt(stream);
            skipComments(stream);

            Index height = readInt(stream);;
            skipComments(stream);

            int pixmax = readInt(stream);

            // find next line
            while(stream.good() && stream.get() != '\n')
            { }

            if(!stream.good())
                throw std::runtime_error("incomplete PPM file");

            if(width <= 0 || height <= 0)
                throw std::runtime_error("invalid PPM image size");

            img.resize(height, width);

            // read image data, data stream stored in row major format
            for(Index r = 0; r < height; ++r)
            {
                for(Index c = 0; c < width; ++c)
                {
                    if(!stream.good())
                        throw std::runtime_error("incomplete PPM image data");

                    uint8_t red = (stream.get() * 255) / pixmax;

                    if(!stream.good())
                        throw std::runtime_error("incomplete PPM image data");

                    uint8_t green = (stream.get() * 255) / pixmax;

                    if(!stream.good())
                        throw std::runtime_error("incomplete PPM image data");

                    uint8_t blue = (stream.get() * 255) / pixmax;

                    img(r, c) = Pixel<RGB>(red, green, blue).template convert<ColorSpace>();
                }
            }
        }
    private:
        void skipComments(std::istream &stream) const
        {
            bool done = false;
            while(!done)
            {
                done = true;
                // trim all white spaces
                while(stream.good() && std::isspace(stream.peek()))
                    stream.get();

                // ignore all comment lines
                while(stream.good() && stream.peek() == '#')
                {
                    // search for end of line
                    while(stream.good() && stream.get() != '\n')
                    { }

                    done = false;
                }
            }
        }

        int readInt(std::istream &stream) const
        {
            int result = 0;
            while(stream.good() && std::isdigit(stream.peek()))
            {
                result = 10 * result + stream.peek() - 48;
                stream.get();
            }

            return result;
        }
    };

    template<>
    class ImageWriter<PPM>
    {
    public:
        template<typename ColorSpace>
        void operator()(std::ostream &stream, const Image<ColorSpace> &img, const PPM &) const
        {
            if(!stream.good())
                throw std::runtime_error("PPM stream is not good");

            stream.write("P6\n", 3);
            stream << img.cols() << ' ' << img.rows() <<'\n';
            stream.write("255\n", 4);

            for(Index r = 0; r < img.rows(); ++r)
            {
                for(Index c = 0; c < img.cols(); ++c)
                {
                    const auto pixel = img(r, c).template convert<RGB>();
                    stream.put(pixel[0]);
                    stream.put(pixel[1]);
                    stream.put(pixel[2]);
                }
            }
        }
    };

}

#endif
