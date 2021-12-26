/* brief_descriptor.h
 *
 * Author: Fabian Meyer
 * Created On: 20 Jun 2019
 */

#ifndef CVE_BRIEF_DESCRIPTOR_H_
#define CVE_BRIEF_DESCRIPTOR_H_

#include <random>
#include "cve/src/core/image.h"

namespace cve
{
    /** BRIEF feature descriptor functor.
      *
      * BRIEF is a binary descriptor which compares the instensity values of
      * pixel pairs.
      * The bit in the result bit mask is set if
      *
      * img[pixA] > img[pixB]
      *
      * The pixel pairs are sampled from a fixed size patch around each keypoint.
      * The pattern of pixel pairs in a patch is determined randomly, but stays
      * consistent across different keypoints.
      */
    template<typename _Scalar>
    class BRIEFDescriptor
    {
    public:
        using Scalar = _Scalar;
        using PatternMatrix = Eigen::Matrix<Index, 4, Eigen::Dynamic>;
        using FeatureMatrix = Eigen::Matrix<Scalar, 2, Eigen::Dynamic>;
        using DescriptorMatrix = Eigen::Matrix<uint32, Eigen::Dynamic, Eigen::Dynamic>;

        BRIEFDescriptor()
        {
            computePattern(256);
        }

        BRIEFDescriptor(const Index length, const Scalar patchSize)
            : _patchSize(patchSize)
        {
            computePattern(length);
        }

        BRIEFDescriptor(const Index length, const Scalar patchSize, const Index seed)
            : _patchSize(patchSize), _seed(seed)
        {
            computePattern(length);
        }

        void setPattern(const PatternMatrix &pattern)
        {
            if(pattern.rows() != 4)
                throw std::runtime_error("BRIEFExtractor pattern must have 4 rows");
            if(pattern.cols() % 32 != 0)
                throw std::runtime_error("BRIEFExtractor pattern columns must be multiple of 32");

            _pattern = pattern;
        }

        const PatternMatrix &pattern() const
        {
            return _pattern;
        }

        Scalar patchSize() const
        {
            return _patchSize;
        }

        Index seed() const
        {
            return _seed;
        }

        /** Computes the descriptors for the given keypoints.
          * @param img input image
          * @param keypoints matrix of keypoints, each column represents a point
          * @param descriptors output matrix of descriptors, each column is a
          *        descriptor for the respective keypoint
          */
        template<typename ColorSpace>
        void operator()(const Image<ColorSpace> &img,
                        const FeatureMatrix &keypoints,
                        DescriptorMatrix &descriptors) const
        {
            static_assert(ColorSpace::Dimension == 1, "BRIEF only supports single channel images");

            descriptors.resize(_pattern.cols() / 32, keypoints.cols());

            descriptors.setZero();
            for(Index c = 0; c < descriptors.cols(); ++c)
            {
                Index x = std::floor(keypoints(0, c));
                Index y = std::floor(keypoints(1, c));

                for(Index i = 0; i < _pattern.cols(); ++i)
                {
                    Index xA = x + _pattern(0, i);
                    Index yA = y + _pattern(1, i);
                    Index xB = x + _pattern(2, i);
                    Index yB = y + _pattern(3, i);

                    if(image::inside(img, yA, xA) &&
                       image::inside(img, yB, xB) &&
                        img(yA, xA)[0] > img(yB, xB)[0])
                    {
                        Index r = i / 32;
                        descriptors(r, c) |= uint32{1} << (31 - (i % 32));
                    }
                }
            }
        }

    private:
        Index _seed = 1297;
        Scalar _patchSize = 32;
        PatternMatrix _pattern = {};

        /** Computes a random pattern for the given patch size with the given
          * number of neighbors.
          * @param length number of neighbors to be computed (must be multiple of 8!)
          * @param patchSize size of the observed area around each keypoint
          * @param seed seed for the random number generator */
        void computePattern(const Index length)
        {
            if(length % 32 != 0)
                throw std::runtime_error("BRIEFExtractor bit length must be multiple of 32");
            if(_patchSize <= 1)
                throw std::runtime_error("BRIEFExtractor patch size must be greater than one");

            _pattern.resize(4, length);

            std::default_random_engine gen(_seed);
            std::uniform_real_distribution<Scalar> distrib(static_cast<Scalar>(-0.5), static_cast<Scalar>(0.5));

            for(Index c = 0; c < _pattern.cols(); ++c)
                for(Index r = 0; r < _pattern.rows(); ++r)
                    _pattern(r, c) = std::floor(distrib(gen) * _patchSize);
        }
    };
}

#endif
