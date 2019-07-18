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
        Matrix pointPairs_;

        void computePointPairs()
        {
            std::default_random_engine gen(seed_);
            std::uniform_real_distribution<Scalar> distrib(-0.5, 0.5);

            for(Index c = 0; c < pointPairs_.cols(); ++c)
                for(Index r = 0; r < pointPairs_.rows(); ++r)
                    pointPairs_(r, c) = distrib(gen) * patchSize_;
        }

        template<typename ScalarA>
        Scalar computeRotationAngle(const Index row, const Index col,
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

        ORBDescriptor(const Index descriptorLength,
            const Scalar patchSize,
            const Index seed = 1297)
            : seed_(seed), patchSize_(patchSize), pointPairs_(4, descriptorLength)
        {
            assert(patchSize > 1);
            assert(descriptorLength % 64 == 0);

            computePointPairs();
        }

        void setPatchSize(const size_t patchSize)
        {
            assert(patchSize > 1);

            patchSize_ = patchSize;
            computePointPairs();
        }

        void setSeed(const Index seed)
        {
            seed_ = seed;
            computePointPairs();
        }

        void setDescriptorLength(const size_t descriptorLength)
        {
            assert(descriptorLength % 64 == 0);

            pointPairs_.resize(4, descriptorLength);
            computePointPairs();
        }

        /**
          *
          */
        template<typename ScalarA>
        void compute(const Eigen::Tensor<ScalarA, 3> &img,
            const Matrix &keypoints,
            Matrixu32 &descriptors) const
        {
            descriptors.resize(pointPairs_.cols() / 32, keypoints.cols());
            Matrix points(pointPairs_.rows(), pointPairs_.cols());

            descriptors.setZero();
            for(Index c = 0; c < descriptors.cols(); ++c)
            {
                Scalar x = keypoints(0, c);
                Scalar y = keypoints(1, c);

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
                points = rot * pointPairs_;

                for(Index i = 0; i < pointPairs_.cols(); ++i)
                {
                    Index xA = std::floor(x + points(0, i));
                    Index yA = std::floor(y + points(1, i));
                    Index xB = std::floor(x + points(2, i));
                    Index yB = std::floor(y + points(3, i));

                    if(image::isInside(yA, xA, img) &&
                        image::isInside(yB, xB, img) &&
                        img(yA, xA, 0) > img(yB, xB, 0))
                    {
                        Index r = i / 32;
                        descriptors(r, c) |= 0x00000001 << (i / 32);
                    }
                }
            }
        }
    };
}

#endif
