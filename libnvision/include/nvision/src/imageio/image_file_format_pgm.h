/* image_file_format_pgm.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef NVISION_IMAGE_FILE_FORMAT_PGM_H_
#define NVISION_IMAGE_FILE_FORMAT_PGM_H_

#include "nvision/src/core/image.h"
#include "nvision/src/imageio/image_file_format.h"

namespace nvision
{
    struct PGM
    { };

    /** Reads an image from a stream in PGM file format. */
    template<>
    class ImageReader<PGM>
    {
    public:
        template<typename Derived>
        void operator()(std::istream &stream, ImageBase<Derived> &img) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
            using ColorSpace = typename ImageBase<Derived>::Scalar::ColorSpace;

            // find start signature "P6"
            while(stream.good() && stream.get() != 'P')
            { }

            if(!stream.good() || stream.get() != '5')
                throw std::runtime_error("missing PGM start sequence");

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
                throw std::runtime_error("incomplete PGM file");

            if(width <= 0 || height <= 0)
                throw std::runtime_error("invalid PGM image size");

            img.resize(height, width);

            // read image data, data stream stored in row major format
            for(Index r = 0; r < height; ++r)
            {
                for(Index c = 0; c < width; ++c)
                {
                    if(!stream.good())
                        throw std::runtime_error("incomplete PGM image data");

                    uint8_t gray = (stream.get() * 255) / pixmax;
                    img(r, c) = Pixel<Gray>(gray).convert<ColorSpace>();
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
    class ImageWriter<PGM>
    {
    public:
        template<typename Derived>
        void operator()(std::ostream &stream, const ImageBase<Derived> &img, const PGM &) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");

            if(!stream.good())
                throw std::runtime_error("PGM stream is not good");

            stream.write("P5\n", 3);
            stream << img.cols() << ' ' << img.rows() <<'\n';
            stream.write("255\n", 4);

            for(Index r = 0; r < img.rows(); ++r)
            {
                for(Index c = 0; c < img.cols(); ++c)
                {
                    const auto pixel = img(r, c).template convert<Gray>();
                    stream.put(pixel[0]);
                }
            }
        }
    };

}

#endif
