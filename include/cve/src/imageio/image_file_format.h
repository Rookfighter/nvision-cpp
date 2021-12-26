/* image_file_format.h
 *
 * Author: Fabian Meyer
 * Created On: 04 Mar 2019
 */

#ifndef CVE_IMAGE_FILE_FORMAT_H_
#define CVE_IMAGE_FILE_FORMAT_H_

#include <istream>
#include <ostream>

namespace cve
{
    template<typename FileFormat>
    class ImageReader { };

    template<typename FileFormat>
    class ImageWriter { };
}

#endif