/* orb_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_ORB_DETECTOR_H_
#define CVE_ORB_DETECTOR_H_

#include "cve/feature/fast_detector_base.h"

namespace cve
{
    /** ORB corner detection functor.
      *
      * This detector basically performs FAST feature detection on a scaled
      * image pyramid.
      * After each feature detection step it scales the image by a given factor
      * and performs another FAST detection. */
    template<typename Scalar, typename Mode=FASTMode9>
    class ORBDetector : public FASTDetectorBase<Scalar, Mode>
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    private:
        Scalar pyramidFactor_;
        Index pyramidLevels_;

    public:
        ORBDetector()
            : ORBDetector(10, 7, 0, true, 0.8, 8)
        { }

        /** Construct an ORB detector with custom parameters.
          * @param threshold FAST threshold which determines if a pixel is brighter or darker
          * @param minDist minimum distance between keypoints in non-maxima suppression
          * @param maxFeatures maximum number of features that should be detected
          * @param useSuppression determine if non-maxima suppression is used
          * @param pyramidFactor scaling factor of the images in the ORB pyramid
          * @param pyramidLevels number of ORB pyramid levels */
        ORBDetector(const Scalar threshold,
            const Index minDist,
            const Index maxFeatures,
            const bool useSuppression,
            const Scalar pyramidFactor,
            const Index pyramidLevels)
            : FASTDetectorBase<Scalar, Mode>(threshold, minDist, maxFeatures,
                useSuppression),
            pyramidFactor_(pyramidFactor), pyramidLevels_(pyramidLevels)
        { }

        /** Set the scale factor for each level of the pyramid created by ORB.
          * If factor is < 1 then the pyramid will consist of downsampled images.
          * If factor is > 1 then the pyramid will consist of upsampled images.
          * @param factor scale factor of each pyramid level */
        void setPyramidFactor(const Scalar factor)
        {
            pyramidFactor_ = factor;
        }

        /** Set the size of the pyramid created by ORB.
          * The image will be resized as many times as the pyramid has levels.
          * @param levels amount of levels of the pyramid */
        void setPyramidLevels(const Index levels)
        {
            pyramidLevels_ = levels;
        }

        /** Compute ORB corner features in the given image.
          * Note that ORB requires a grayscale image as input!
          * The keypoints are returned in a matrix, where each column
          * represents a single 2D point. Each keypoint is stored in (x y), i.e.
          * (column row), format.
          * @param img input grayscale image
          * @param keypoints 2xN matrix with N keypoints */
        template<typename ScalarA>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            Matrix &keypoints) const
        {
            if(img.dimension(2) > 1)
                throw std::runtime_error("ORB can only compute single channel images");

            Eigen::Tensor<ScalarA, 3> imgScaled = img;
            std::vector<Vector2i> corners;
            corners.reserve(5000 * pyramidLevels_);
            Scalar scale = 1;

            // perform FAST detection for each level of the pyramid
            for(Index i = 0; i < pyramidLevels_; ++i)
            {
                size_t sizeBefore = corners.size();
                // detect all corners according to the current mode of operation
                this->detectCorners(imgScaled, corners);
                size_t sizeAfter = corners.size();
                for(size_t j = sizeBefore; j < sizeAfter; ++j)
                {
                    corners[j](0) = static_cast<Scalar>(corners[j](0)) / scale;
                    corners[j](1) = static_cast<Scalar>(corners[j](1)) / scale;
                }

                // scale image to build pyramid
                image::resize(pyramidFactor_, imgScaled);
                scale *= pyramidFactor_;
            }

            // compute score only if non-maxima suppression is used or we have
            // found more corners than allowed
            Matrix scoreMat;
            if(this->useSuppression_ || (this->maxFeatures_ > 0 && static_cast<Index>(corners.size()) > this->maxFeatures_))
            {
                // compute the cornerness score for each detected corner
                this->computeScoreMatrix(img, corners, scoreMat);
            }

            if(this->useSuppression_)
            {
                // perform non-maxima suppression to make features sparser
                // basically maintain a minimum distance between feature points
                this->nonMaximaSuppression(scoreMat, corners);
            }

            Index featureCnt = static_cast<Index>(corners.size());
            if(this->maxFeatures_ > 0)
                featureCnt = std::min(this->maxFeatures_, featureCnt);

            // check if maximum features has been reached
            if(featureCnt != static_cast<Index>(corners.size()))
            {
                std::sort(corners.begin(), corners.end(),
                    [&scoreMat](const Vector2i &lhs, const Vector2i &rhs)
                    { return scoreMat(lhs(1), lhs(0)) < scoreMat(rhs(1), rhs(0)); });
            }

            // copy detected points into output matrix
            keypoints.resize(2, featureCnt);
            for(Index i = 0; i < keypoints.cols(); ++i)
                keypoints.col(i) = corners[i].template cast<Scalar>();
        }
    };
}

#endif
