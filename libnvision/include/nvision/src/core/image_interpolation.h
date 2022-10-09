/* image_interpolation.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef NVISION_IMAGE_INTERPOLATION_H_
#define NVISION_IMAGE_INTERPOLATION_H_

#include "nvision/src/core/image_type.h"
#include "nvision/src/core/math.h"

namespace nvision
{
    /** Computes the vertical linear interpolation at the given image coordinates. */
    struct LinearVerticalInterpolation
    {
        template<typename Derived, typename Float>
        typename ImageBase<Derived>::Scalar operator()(const ImageBase<Derived> &img,
                                           const Float row,
                                           const Index col) const
        {
            static_assert(Eigen::NumTraits<Float>::IsInteger == 0, "indexed values must be floating point");
            static_assert(IsImage<ImageBase<Derived>>::value, "input is not a valid image type");
            using PixelType = typename ImageBase<Derived>::Scalar;
            using ColorSpace = typename PixelType::ColorSpace;

            const auto r1 = std::floor(row);
            const auto r2 = std::ceil(row);

            const auto r1idx = clamp<Index>(static_cast<Index>(r1), 0, img.rows() - 1);
            const auto r2idx = clamp<Index>(static_cast<Index>(r2), 0, img.rows() - 1);

            const auto &pixel1 = img(r1idx, col);
            const auto &pixel2 = img(r2idx, col);

            // calculate factors in y direction
            auto vfac1 = Float{1};
            auto vfac2 = Float{0};
            if(r2 != r1)
            {
                vfac1 = (r2 - row) / (r2 - r1);
                vfac2 = (row - r1) / (r2 - r1);
            }

            auto result = PixelType{};
            for(Index i = 0; i < result.size(); ++i)
            {
                result[i] = static_cast<typename ColorSpace::ValueType>(
                                vfac1 * static_cast<Float>(pixel1[i]) +
                                vfac2 * static_cast<Float>(pixel2[i]));
            }
            return result;
        }
    };

    /** Computes the horizontal linear interpolation at the given image coordinates. */
    struct LinearHorizontalInterpolation
    {
        template<typename Derived, typename Float>
        typename ImageBase<Derived>::Scalar operator()(const ImageBase<Derived> &img,
                                           const Index row,
                                           const Float col) const
        {
            static_assert(Eigen::NumTraits<Float>::IsInteger == 0, "indexed values must be floating point");
            static_assert(IsImage<ImageBase<Derived>>::value, "input is not a valid image type");
            using PixelType = typename ImageBase<Derived>::Scalar;
            using ColorSpace = typename PixelType::ColorSpace;

            const auto c1 = std::floor(col);
            const auto c2 = std::ceil(col);

            const auto c1idx = clamp<Index>(static_cast<Index>(c1), 0, img.cols() - 1);
            const auto c2idx = clamp<Index>(static_cast<Index>(c2), 0, img.cols() - 1);

            const auto &pixel1 = img(row, c1idx);
            const auto &pixel2 = img(row, c2idx);

            // calculate factors in x direction
            auto hfac1 = Float{1};
            auto hfac2 = Float{0};
            if(c2 != c1)
            {
                hfac1 = (c2 - col) / (c2 - c1);
                hfac2 = (col - c1) / (c2 - c1);
            }

            auto result = PixelType{};
            for(Index i = 0; i < result.size(); ++i)
            {
                result[i] = static_cast<typename ColorSpace::ValueType>(
                                hfac1 * static_cast<Float>(pixel1[i]) +
                                hfac2 * static_cast<Float>(pixel2[i]));
            }
            return result;
        }
    };

    /** Computes the bilinear interpolation at the given image coordinates. */
    struct BilinearInterpolation
    {
        template<typename Derived, typename Float>
        typename ImageBase<Derived>::Scalar operator()(const ImageBase<Derived> &img,
                                           const Float row,
                                           const Float col) const
        {
            static_assert(Eigen::NumTraits<Float>::IsInteger == 0, "indexed values must be floating point");
            static_assert(IsImage<ImageBase<Derived>>::value, "input is not a valid image type");
            using PixelType = typename ImageBase<Derived>::Scalar;
            using ColorSpace = typename PixelType::ColorSpace;

            const auto r1 = std::floor(row);
            const auto r2 = std::ceil(row);
            const auto c1 = std::floor(col);
            const auto c2 = std::ceil(col);

            const auto r1idx = clamp<Index>(static_cast<Index>(r1), 0, img.rows() - 1);
            const auto r2idx = clamp<Index>(static_cast<Index>(r2), 0, img.rows() - 1);
            const auto c1idx = clamp<Index>(static_cast<Index>(c1), 0, img.cols() - 1);
            const auto c2idx = clamp<Index>(static_cast<Index>(c2), 0, img.cols() - 1);

            const auto &pixel11 = img(r1idx, c1idx);
            const auto &pixel12 = img(r1idx, c2idx);
            const auto &pixel21 = img(r2idx, c1idx);
            const auto &pixel22 = img(r2idx, c2idx);

            // calculate factors in x direction
            auto hfac1 = Float{1};
            auto hfac2 = Float{0};
            if(c2 != c1)
            {
                hfac1 = (c2 - col) / (c2 - c1);
                hfac2 = (col - c1) / (c2 - c1);
            }

            // calculate factors in y direction
            auto vfac1 = Float{1};
            auto vfac2 = Float{0};
            if(r2 != r1)
            {
                vfac1 = (r2 - row) / (r2 - r1);
                vfac2 = (row - r1) / (r2 - r1);
            }

            const auto f11 = hfac1 * vfac1;
            const auto f12 = hfac2 * vfac1;
            const auto f21 = hfac1 * vfac2;
            const auto f22 = hfac2 * vfac2;

            auto result = PixelType{};
            for(Index i = 0; i < result.size(); ++i)
            {
                result[i] = static_cast<typename ColorSpace::ValueType>(
                                f11 * static_cast<Float>(pixel11[i]) +
                                f12 * static_cast<Float>(pixel12[i]) +
                                f21 * static_cast<Float>(pixel21[i]) +
                                f22 * static_cast<Float>(pixel22[i]));
            }
            return result;
        }
    };

    namespace image
    {

        /** Computes the vertical linear interpolation at the given image coordinates.
          * @param img source image used to compute the interpolation values
          * @param row row index of the interpolated pixel
          * @param col column index of the interpolated pixel
          * @return interpolated pixel value */
        template<typename Derived, typename Float>
        inline typename ImageBase<Derived>::Scalar interpolateLinearVertical(const ImageBase<Derived> &img,
                                              const Float row,
                                              const Index col)
        {
            const LinearVerticalInterpolation interpolation;
            return interpolation(img, row, col);
        }

        /** Computes the horizontal linear interpolation at the given image coordinates.
          * @param img source image used to compute the interpolation values
          * @param row row index of the interpolated pixel
          * @param col column index of the interpolated pixel
          * @return interpolated pixel value */
        template<typename Derived, typename Float>
        inline typename ImageBase<Derived>::Scalar interpolateLinearHorizontal(const ImageBase<Derived> &img,
                                              const Index row,
                                              const Float col)
        {
            const LinearHorizontalInterpolation interpolation;
            return interpolation(img, row, col);
        }

        /** Computes the bilinear interpolation at the given image coordinates.
          * @param img source image used to compute the interpolation values
          * @param row row index of the interpolated pixel
          * @param col column index of the interpolated pixel
          * @return interpolated pixel value */
        template<typename Derived, typename Float>
        inline typename ImageBase<Derived>::Scalar interpolateBilinear(const ImageBase<Derived> &img,
                                              const Float row,
                                              const Float col)
        {
            const BilinearInterpolation interpolation;
            return interpolation(img, row, col);
        }
    }
}

#endif
