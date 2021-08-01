/* brief_extractor.h
 *
 * Author: Fabian Meyer
 * Created On: 20 Jun 2019
 */

#ifndef CVE_BRIEF_EXTRACTOR_H_
#define CVE_BRIEF_EXTRACTOR_H_

#include <random>
#include "cve/core/image.h"

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
    template<typename Scalar>
    class BRIEFExtractor
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    private:
        Index seed_;
        Scalar patchSize_;
        Matrixi pattern_;

    public:

        BRIEFExtractor()
            : BRIEFExtractor(256, 31, 1297)
        {

        }

        BRIEFExtractor(const Index length,
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
            const Index seed)
        {
            if(length % 32 != 0)
                throw std::runtime_error("BRIEFExtractor bit length must be multiple of 32");
            if(patchSize <= 1)
                throw std::runtime_error("BRIEFExtractor patch size must be greater than one");

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
                throw std::runtime_error("BRIEFExtractor pattern must have 4 rows");
            if(pattern.cols() % 32 != 0)
                throw std::runtime_error("BRIEFExtractor pattern columns must be multiple of 32");

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

        /** Computes the descriptors for the given keypoints.
          * @param img input image
          * @param keypoints matrix of keypoints, each column represents a point
          * @param descriptors output matrix of descriptors, each column is a
          *        descriptor for the respective keypoint
          */
        template<typename ScalarA>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const Matrix &keypoints,
            Matrixi32 &descriptors) const
        {
            if(img.dimension(2) > 1)
                throw std::runtime_error("BRIEF can only compute single channel images");

            descriptors.resize(pattern_.cols() / 32, keypoints.cols());

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
                        Index r = i / 32;
                        descriptors(r, c) |= 0x00000001 << (31 - (i % 32));
                    }
                }
            }
        }
    };
}

#endif
