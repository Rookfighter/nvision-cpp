/* orb_descriptor.h
 *
 * Author: Fabian Meyer
 * Created On: 20 Jun 2019
 */

#ifndef NVISION_ORB_DESCRIPTOR_H_
#define NVISION_ORB_DESCRIPTOR_H_

#include <random>
#include "nvision/src/core/image.h"

namespace nvision
{
    /** ORB feature descriptor functor.
      *
      * ORB is a binary descriptor, which compares the instensity values of
      * pixel pairs, similar to BRIEF. ORB also adds a rotation compensation
      * to make it rotation invariant.
      *
      * The pixel pairs are sampled from a fixed size patch around each keypoint.
      * The pattern of pixel pairs in a patch is determined randomly, but stays
      * consistent across different keypoints. */
    template<typename _Scalar>
    class ORBDescriptor
    {
    public:
        using Scalar = _Scalar;
        using PatternMatrix = Eigen::Matrix<Index, 4, Eigen::Dynamic>;
        using BasePatternMatrix = Eigen::Matrix<Scalar, 4, Eigen::Dynamic>;
        using FeatureMatrix = Eigen::Matrix<Scalar, 2, Eigen::Dynamic>;
        using DescriptorMatrix = Eigen::Matrix<uint32, Eigen::Dynamic, Eigen::Dynamic>;
        using Matrix4 = Eigen::Matrix<Scalar, 4, 4>;

        ORBDescriptor()
        {
            computePattern(256);
        }

        ORBDescriptor(const Index length, const Scalar patchSize)
            : _patchSize(patchSize)
        {
            computePattern(length);
        }

        ORBDescriptor(const Index length, const Scalar patchSize, const Index seed)
            : _seed(seed), _patchSize(patchSize)
        {
            computePattern(length);
        }

        void setPattern(const BasePatternMatrix &pattern)
        {
            if(pattern.rows() != 4)
                throw std::runtime_error("ORBExtractor pattern must have 4 rows");
            if(pattern.cols() % 32 != 0)
                throw std::runtime_error("ORBExtractor pattern columns must be multiple of 32");

            _basePattern = pattern;
            computeRotatedPatterns();
        }

        const BasePatternMatrix &pattern() const
        {
            return _basePattern;
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
            static_assert(ColorSpace::Dimension == 1, "ORB only supports single channel images");

            descriptors.resize(_basePattern.cols() / 32, keypoints.cols());

            descriptors.setZero();
            for(Index c = 0; c < descriptors.cols(); ++c)
            {
                const auto x = static_cast<Index>(keypoints(0, c));
                const auto y = static_cast<Index>(keypoints(1, c));

                // compute which pattern should be used here
                const auto idx = calcPatternIndex(img, y, x);
                const auto &pattern = _patterns[idx];

                for(Index i = 0; i < pattern.cols(); ++i)
                {
                    const auto xA = x + pattern(0, i);
                    const auto yA = y + pattern(1, i);
                    const auto xB = x + pattern(2, i);
                    const auto yB = y + pattern(3, i);

                    if(image::inside(img, yA, xA) &&
                       image::inside(img, yB, xB) &&
                       img(yA, xA)[0] > img(yB, xB)[0])
                    {
                        const auto r = i / 32;
                        descriptors(r, c) |= 0x01 << (31 - (i % 32));
                    }
                }
            }
        }

    private:
        Index _seed = 1297;
        Scalar _patchSize = 31;
        BasePatternMatrix _basePattern = {};
        std::vector<PatternMatrix> _patterns = {};
        Index _partitions = 12;

        /** Computes a random pattern for the given patch size with the given
          * number of neighbors.
          * @param length number of neighbors to be computed (must be multiple of 32!)
          * @param patchSize size of the observed area around each keypoint
          * @param seed seed for the random number generator */
        void computePattern(const Index length)
        {
            if(length % 32 != 0)
                throw std::runtime_error("ORB bit length must be multiple of 32");
            if(_patchSize <= 1)
                throw std::runtime_error("ORB patch size must be greater than one");

            computeRandomPattern(length);
            computeRotatedPatterns();
        }

        void computeRandomPattern(const Index length)
        {
            // compute pattern randomly
            _basePattern.resize(4, length);
            std::default_random_engine gen(_seed);
            std::uniform_real_distribution<Scalar> distrib(static_cast<Scalar>(-0.5), static_cast<Scalar>(0.5));
            for(Index c = 0; c < _basePattern.cols(); ++c)
                for(Index r = 0; r < _basePattern.rows(); ++r)
                    _basePattern(r, c) = distrib(gen) * _patchSize;
        }

        void computeRotatedPatterns()
        {
            // compute discretized rotated patterns
            const auto anglePartition = 2 * pi<Scalar>() / static_cast<Scalar>(_partitions);
            _patterns.resize(_partitions);
            for(size_t i = 0; i < _patterns.size(); ++i)
            {
                auto &pattern = _patterns[i];
                const auto theta = static_cast<Scalar>(i) * anglePartition;
                const auto sinVal = std::sin(theta);
                const auto cosVal = std::cos(theta);
                Matrix4 rot;
                rot << cosVal, -sinVal, 0, 0,
                       sinVal, cosVal, 0, 0,
                       0, 0, cosVal, -sinVal,
                       0, 0, sinVal, cosVal;
                pattern.resize(_basePattern.rows(), _basePattern.cols());
                pattern = (rot * _basePattern).template cast<Index>();
            }
        }

        template<typename ColorSpace>
        Index calcPatternIndex(const Image<ColorSpace> &img,
                               const Index row,
                               const Index col) const
        {
            const auto patchSize = static_cast<Index>(_patchSize);
            auto m01 = Scalar{0};
            auto m10 = Scalar{0};
            for(Index cOff = -patchSize; cOff < patchSize + 1; ++cOff)
            {
                Index c2 = col + cOff;
                for(Index rOff = -patchSize; rOff < patchSize + 1; ++rOff)
                {
                    Index r2 = row + rOff;
                    if(image::inside(img, r2, c2))
                    {
                        m01 += rOff * img(r2, c2)[0];
                        m10 += cOff * img(r2, c2)[0];
                    }
                }
            }

            const auto theta = angle::normalize(std::atan2(m01, m10));
            return static_cast<Index>(theta / (2 * pi<Scalar>() / _partitions));
        }
    };
}

#endif
