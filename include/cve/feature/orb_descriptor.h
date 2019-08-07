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
    template<typename Scalar>
    class ORBDescriptor
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, 4, 4> Matrix4;

    private:
        Index seed_;
        Scalar patchSize_;
        Matrix pattern_;

        template<typename ScalarA>
        Scalar computeRotationAngle(const Index row,
            const Index col,
            const Eigen::Tensor<ScalarA, 3> &img) const
        {
            Scalar m01 = 0;
            Scalar m10 = 0;
            for(Index c2 = col - patchSize_; c2 < col + patchSize_ + 1; ++c2)
            {
                for(Index r2 = row - patchSize_; r2 < row + patchSize_ + 1; ++r2)
                {
                    if(image::isInside(r2, c2, img))
                    {
                        m01 += r2 * img(r2, c2, 0);
                        m10 += c2 * img(r2, c2, 0);
                    }
                }
            }

            // discretize angle to a certain partitioning
            // 2 * pi / 30
            Scalar anglePartition = 0.2094395102393195;
            Scalar theta = std::atan2(m01, m10);
            theta = std::floor(theta / anglePartition) * anglePartition;
            return theta;
        }
    public:

        ORBDescriptor()
            : ORBDescriptor(128, 16, 1297)
        {

        }

        ORBDescriptor(const Index length,
            const Scalar patchSize,
            const Index seed = 1297)
            : seed_(), patchSize_(), pattern_()
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
            pattern_.resize(4, length);

            std::default_random_engine gen(seed);
            std::uniform_real_distribution<Scalar> distrib(-0.5, 0.5);

            for(Index c = 0; c < pattern_.cols(); ++c)
                for(Index r = 0; r < pattern_.rows(); ++r)
                    pattern_(r, c) = std::floor(distrib(gen) * patchSize);
        }

        void setPattern(const Matrixi &pattern)
        {
            if(pattern.rows() != 4)
                throw std::runtime_error("ORBDescriptor pattern must have 4 rows");
            if(pattern.cols() % 8 != 0)
                throw std::runtime_error("ORBDescriptor pattern columns must be multiple of 8");

            pattern_ = pattern;
        }

        const Matrixi &pattern() const
        {
            return pattern_;
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
            descriptors.resize(pattern_.cols() / 8, keypoints.cols());
            Matrix points(pattern_.rows(), pattern_.cols());

            descriptors.setZero();
            for(Index c = 0; c < descriptors.cols(); ++c)
            {
                Index x = std::floor(keypoints(0, c));
                Index y = std::floor(keypoints(1, c));

                // compute a rotation angle for rotation invariance
                Scalar theta = computeRotationAngle(y, x, img);

                // compute rotation matrix and rotate point pairs
                Scalar sinVal = std::sin(theta);
                Scalar cosVal = std::cos(theta);
                Matrix4 rot;
                rot << cosVal, -sinVal, 0, 0,
                       sinVal, cosVal, 0, 0,
                       0, 0, cosVal, -sinVal,
                       0, 0, sinVal, cosVal;
                points = rot * pattern_;

                for(Index i = 0; i < points.cols(); ++i)
                {
                    Index xA = x + std::floor(points(0, i));
                    Index yA = y + std::floor(points(1, i));
                    Index xB = x + std::floor(points(2, i));
                    Index yB = y + std::floor(points(3, i));

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
