/* image_file_format_jpeg.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef NVISION_IMAGE_FILE_FORMAT_JPEG_H_
#define NVISION_IMAGE_FILE_FORMAT_JPEG_H_

#include "nvision/src/core/image.h"
#include "nvision/src/imageio/image_file_format.h"

namespace nvision
{
    struct JPEG
    {
        /** Quality of the JPEG file. Must lie in [1;100] */
        uint32 quality = uint32{1};
    };


    template<>
    class ImageReader<JPEG>
    {
    public:
        template<typename Derived>
        void operator()(std::istream &stream, ImageBase<Derived> &img) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
        }
    };

    template<>
    class ImageWriter<JPEG>
    {
    public:
        template<typename Derived>
        void operator()(std::ostream &stream, const ImageBase<Derived> &img, const JPEG &options) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
        }
    };
}

#endif
