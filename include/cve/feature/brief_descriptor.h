/* brief_descriptor.h
 *
 * Author: Fabian Meyer
 * Created On: 20 Jun 2019
 */

#ifndef CVE_BRIEF_DESCRIPTOR_H_
#define CVE_BRIEF_DESCRIPTOR_H_

#include <random>
#include "cve/image.h"

namespace cve
{
    template<typename Scalar>
    class BRIEFDescriptor
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu8;

    private:
        ssize_t seed_;
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
    public:

        BRIEFDescriptor()
            : BRIEFDescriptor(128, 16, 1297)
        {

        }

        BRIEFDescriptor(const size_t descriptorLength,
            const Scalar patchSize,
            const ssize_t seed = 1297)
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

        void setSeed(const ssize_t seed)
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
            Matrixu8 &descriptors) const
        {
            descriptors.resize(pointPairs_.cols() / 8, keypoints.cols());

            descriptors.setZero();
            for(Index c = 0; c < descriptors.cols(); ++c)
            {
                Scalar x = keypoints(0, c);
                Scalar y = keypoints(1, c);

                for(Index i = 0; i < pointPairs_.cols(); ++i)
                {
                    Index xA = std::floor(x + pointPairs_(0, i));
                    Index yA = std::floor(y + pointPairs_(1, i));
                    Index xB = std::floor(x + pointPairs_(2, i));
                    Index yB = std::floor(y + pointPairs_(3, i));

                    if(image::isInside(yA, xA, img) &&
                        image::isInside(yB, xB, img) &&
                        img(yA, xA, 0) > img(yB, xB, 0))
                    {
                        Index r = i / 8;
                        descriptors(r, c) |= 0x01 << (i % 8);
                    }
                }
            }
        }
    };
}

#endif
