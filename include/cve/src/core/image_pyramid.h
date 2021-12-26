/* image_pyramid.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_IMAGE_PYRAMID_H_
#define CVE_IMAGE_PYRAMID_H_

#include "cve/src/core/image_type.h"
#include "cve/src/core/image_resizing.h"

namespace cve
{
    /** Data representation of an image pyramid.
      * Provides functionality to compute and access image pyramids of different scaling. */
    template<typename _Scalar, typename _ColorSpace>
    class ImagePyramid
    {
    public:
        using Scalar = _Scalar;
        using ColorSpace = _ColorSpace;

        static_assert(Eigen::NumTraits<Scalar>::IsInteger == 0, "Image pyramid must use floating points");

        /** Single level of an image pyramid which specifies the absolute scale
          * to the original image in horizontal and vertical direction. */
        struct Level
        {
            Scalar scaleX;
            Scalar scaleY;
        };

        ImagePyramid(const Image<ColorSpace> &img, const Index levels, const Scalar factor)
            : ImagePyramid(img, levels, factor, factor)
        { }

        ImagePyramid(const Image<ColorSpace> &img, const Index levels, const Scalar factorX , const Scalar factorY)
        {
            _levels.resize(levels);

            auto scaleX = Scalar{1};
            auto scaleY = Scalar{1};
            for(size_t i = 0; i < _levels.size(); ++i)
            {
                _levels[i].scaleX = scaleX;
                _levels[i].scaleY = scaleY;
                scaleX *= factorX;
                scaleY *= factorY;
            }

            computeImages(img);
        }

        ImagePyramid(const Image<ColorSpace> &img, const std::vector<Level> &levels)
            : _levels(levels)
        {
            computeImages(img);
        }

        const std::vector<Image<ColorSpace>> &images() const
        {
            return _images;
        }

        const std::vector<Level> &levels() const
        {
            return _levels;
        }

        Index size() const
        {
            return static_cast<Index>(_levels.size());
        }

    private:
        std::vector<Level> _levels = {};
        std::vector<Image<ColorSpace>> _images = {};

        void computeImages(const Image<ColorSpace> &img)
        {
            _images.resize(_levels.size());
            for(size_t i = 0; i < _levels.size(); ++i)
            {
                const auto &level = _levels[i];
                if(Scalar{1} == level.scaleX && Scalar{1} == level.scaleY)
                {
                    _images[i] = img;
                }
                else
                {
                    _images[i] = image::resize(img, level.scaleX, level.scaleY);
                }
            }
        }
    };

    namespace image
    {
        /** Creates an image pyramid with a uniform horizontal and vertical scale.
          * @param img the original image which should be used in the pyramid
          * @param levels the number of levels that the pyramid should contain
          * @param factor the unform scaling factor that will be applied to create the pyramid
          * @return the generated image pyramid. */
        template<typename Scalar, typename ColorSpace>
        ImagePyramid<Scalar, ColorSpace> makePyramid(const Image<ColorSpace> &img,
                                                     const Index levels,
                                                     const Scalar factor)
        {
            return ImagePyramid<Scalar, ColorSpace>(img, levels, factor);
        }

        /** Creates an image pyramid with specified horizontal and vertical scale.
          * @param img the original image which should be used in the pyramid
          * @param levels the number of levels that the pyramid should contain
          * @param factorX the scaling factor that will be applied horizontally
          * @param factorY the scaling factor that will be applied vertically
          * @return the generated image pyramid. */
        template<typename Scalar, typename ColorSpace>
        ImagePyramid<Scalar, ColorSpace> makePyramid(const Image<ColorSpace> &img,
                                                     const Index levels,
                                                     const Scalar factorX,
                                                     const Scalar factorY)
        {
            return ImagePyramid<Scalar, ColorSpace>(img, levels, factorX, factorY);
        }

        /** Creates an image pyramid with specified levels and their independent scales.
          * @param img the original image which should be used in the pyramid
          * @param levels list of levels that the pyramid should contain
          * @return the generated image pyramid. */
        template<typename Scalar, typename ColorSpace>
        ImagePyramid<Scalar, ColorSpace> makePyramid(const Image<ColorSpace> &img,
                                                     const std::vector<typename ImagePyramid<Scalar, ColorSpace>::Level> &levels)
        {
            return ImagePyramid<Scalar, ColorSpace>(img, levels);
        }
    }
}

#endif