/* brief_descriptor.h
 *
 * Author: Fabian Meyer
 * Created On: 20 Jun 2019
 */

#ifndef CVE_BRIEF_DESCRIPTOR_H_
#define CVE_BRIEF_DESCRIPTOR_H_

#include <random>
#include "cve/core/image.h"

namespace cve
{
    template<typename Scalar>
    class BRIEFDescriptor
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    private:
        Index seed_;
        Scalar patchSize_;
        Matrixi pattern_;

    public:

        BRIEFDescriptor()
            : BRIEFDescriptor(128, 31, 1297)
        {

        }

        BRIEFDescriptor(const Index bitLength,
            const Scalar patchSize,
            const Index seed = 1297)
            : seed_(), patchSize_(), pattern_()
        {
            computePattern(bitLength, patchSize, seed);
        }

        void computePattern(const Index bitLength,
            const Scalar patchSize,
            const Index seed)
        {
            if(bitLength % 8 != 0)
                throw std::runtime_error("BRIEF bit length must be multiple of 8");
            if(patchSize <= 1)
                throw std::runtime_error("BRIEF patch size must be greater than one");

            seed_ = seed;
            patchSize_ = patchSize;
            pattern_.resize(4, bitLength);

            std::default_random_engine gen(seed);
            std::uniform_real_distribution<Scalar> distrib(-0.5, 0.5);

            for(Index c = 0; c < pattern_.cols(); ++c)
                for(Index r = 0; r < pattern_.rows(); ++r)
                    pattern_(r, c) = std::floor(distrib(gen) * patchSize);
        }

        void setPattern(const Matrixi &pattern)
        {
            if(pattern.rows() != 4)
                throw std::runtime_error("BRIEF pattern must have 4 rows");
            if(pattern.cols() % 8 != 0)
                throw std::runtime_error("BRIEF pattern columns must be multiple of 8");

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

            descriptors.setZero();
            for(Index c = 0; c < descriptors.cols(); ++c)
            {
                Index x = std::floor(keypoints(0, c));
                Index y = std::floor(keypoints(1, c));

                for(Index i = 0; i < pattern_.cols(); ++i)
                {
                    Index xA = x + pattern_(0, i);
                    Index yA = y + pattern_(1, i);
                    Index xB = x + pattern_(2, i);
                    Index yB = y + pattern_(3, i);

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
