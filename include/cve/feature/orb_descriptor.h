/* orb_descriptor.h
 *
 * Author: Fabian Meyer
 * Created On: 20 Jun 2019
 */

#ifndef CVE_ORB_DESCRIPTOR_H_
#define CVE_ORB_DESCRIPTOR_H_

#include <random>
#include "cve/core/image.h"

namespace cve
{
    /** Class to compute ORB feature descriptors.
      *
      * ORB is a binary descriptor, which compares the instensity values of
      * pixel pairs, similar to BRIEF. ORB also adds a rotation compensation
      * to make it rotation invariant.
      *
      * The pixel pairs are sampled from a fixed size patch around each keypoint.
      * The pattern of pixel pairs in a patch is determined randomly, but stays
      * consistent across different keypoints. */
    template<typename Scalar>
    class ORBDescriptor
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, 4, 4> Matrix4;

    private:
        Index seed_;
        Scalar patchSize_;
        std::vector<Matrixi> patterns_;
        Index partitions_;

        void computeRandomPattern(const Index length,
            const Scalar patchSize,
            const Index seed,
            Matrix &pattern)
        {
            // compute pattern randomly
            pattern.resize(4, length);
            std::default_random_engine gen(seed);
            std::uniform_real_distribution<Scalar> distrib(-0.5, 0.5);
            for(Index c = 0; c < pattern.cols(); ++c)
                for(Index r = 0; r < pattern.rows(); ++r)
                    pattern(r, c) = distrib(gen) * patchSize;
        }

        void computeRotatedPatterns(const Matrix &pattern,
            std::vector<Matrixi> &patterns) const
        {
            // compute discretized rotated patterns
            Scalar anglePartition = 2 * pi<Scalar>() / static_cast<Scalar>(partitions_);
            patterns.resize(partitions_);
            for(size_t i = 0; i < patterns.size(); ++i)
            {
                Scalar theta = static_cast<Scalar>(i) * anglePartition;
                Scalar sinVal = std::sin(theta);
                Scalar cosVal = std::cos(theta);
                Matrix4 rot;
                rot << cosVal, -sinVal, 0, 0,
                       sinVal, cosVal, 0, 0,
                       0, 0, cosVal, -sinVal,
                       0, 0, sinVal, cosVal;
                patterns[i].resize(pattern.rows(), pattern.cols());
                patterns[i] = (rot * pattern).template cast<Index>();
            }
        }

        template<typename ScalarA>
        Index calcPatternIndex(const Index row,
            const Index col,
            const Eigen::Tensor<ScalarA, 3> &img) const
        {
            Index patchSize = static_cast<Index>(patchSize_);
            Scalar m01 = 0;
            Scalar m10 = 0;
            for(Index cOff = -patchSize; cOff < patchSize + 1; ++cOff)
            {
                Index c2 = col + cOff;
                for(Index rOff = -patchSize; rOff < patchSize + 1; ++rOff)
                {
                    Index r2 = row + rOff;
                    if(image::isInside(r2, c2, img))
                    {
                        m01 += rOff * img(r2, c2, 0);
                        m10 += cOff * img(r2, c2, 0);
                    }
                }
            }

            Scalar theta = angle::normalize(std::atan2(m01, m10));
            return static_cast<Index>(theta / (2 * pi<Scalar>() / partitions_));
        }
    public:

        ORBDescriptor()
            : ORBDescriptor(256, 31, 1297)
        {

        }

        ORBDescriptor(const Index length,
            const Scalar patchSize,
            const Index seed = 1297)
            : seed_(), patchSize_(), patterns_(), partitions_(12)
        {
            computePattern(length, patchSize, seed);
        }

        /** Computes a random pattern for the given patch size with the given
          * number of neighbors.
          * @param length number of neighbors to be computed (must be multiple of 8!)
          * @param patchSize size of the observed area around each keypoint
          * @param seed seed for the random number generator */
        void computePattern(const Index length,
            const Scalar patchSize,
            const Index seed = 1297)
        {
            if(length % 8 != 0)
                throw std::runtime_error("ORBDescriptor bit length must be multiple of 8");
            if(patchSize <= 1)
                throw std::runtime_error("ORBDescriptor patch size must be greater than one");

            seed_ = seed;
            patchSize_ = patchSize;

            Matrix pattern;
            computeRandomPattern(length, patchSize, seed, pattern);
            computeRotatedPatterns(pattern, patterns_);
        }

        void setPattern(const Matrix &pattern)
        {
            if(pattern.rows() != 4)
                throw std::runtime_error("ORBDescriptor pattern must have 4 rows");
            if(pattern.cols() % 8 != 0)
                throw std::runtime_error("ORBDescriptor pattern columns must be multiple of 8");

            computeRotatedPatterns(pattern);
        }

        const Matrixi &pattern() const
        {
            return patterns_[0];
        }

        Scalar patchSize() const
        {
            return patchSize_;
        }

        Index seed() const
        {
            return seed_;
        }

        /**
          *
          */
        template<typename ScalarA>
        void compute(const Eigen::Tensor<ScalarA, 3> &img,
            const Matrix &keypoints,
            Matrixu8 &descriptors) const
        {
            if(img.dimension(2) > 1)
                throw std::runtime_error("ORB can only compute single channel images");

            descriptors.resize(patterns_[0].cols() / 8, keypoints.cols());

            descriptors.setZero();
            for(Index c = 0; c < descriptors.cols(); ++c)
            {
                Index x = static_cast<Index>(keypoints(0, c));
                Index y = static_cast<Index>(keypoints(1, c));

                // compute which pattern should be used here
                Index idx = calcPatternIndex(y, x, img);
                const Matrixi &pattern = patterns_[idx];

                for(Index i = 0; i < pattern.cols(); ++i)
                {
                    Index xA = x + pattern(0, i);
                    Index yA = y + pattern(1, i);
                    Index xB = x + pattern(2, i);
                    Index yB = y + pattern(3, i);

                    if(image::isInside(yA, xA, img) &&
                        image::isInside(yB, xB, img) &&
                        img(yA, xA, 0) > img(yB, xB, 0))
                    {
                        Index r = i / 8;
                        descriptors(r, c) |= 0x01 << (7 - (i % 8));
                    }
                }
            }
        }
    };
}

#endif
