/* fast_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_FAST_DETECTOR_H_
#define CVE_FAST_DETECTOR_H_

#include "cve/feature/fast_detector_base.h"

namespace cve
{
    /** FAST corner detection functor.
      *
      * This corner detector essentially checks the pixels around a potential
      * keypoint to be either lighter or darker than the keypoint itself.
      * If the pixels on the circle form a long enough sequence, the keypoint
      * is accepted. */
    template<typename Scalar, typename Mode=FASTMode7, typename Score=FASTScore<Scalar, Mode>>
    class FASTDetector : public FASTDetectorBase<Scalar, Mode, Score>
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    public:
        FASTDetector()
            : FASTDetector(20, 16, 0, true)
        { }

        /** Construct a FAST detector with custom parameters.
          * @param threshold FAST threshold which determines if a pixel is brighter or darker
          * @param minDist minimum distance between keypoints in non-maxima suppression
          * @param maxFeatures maximum number of features that should be detected
          * @param useSuppresion determine if non-maxima suppression is used */
        FASTDetector(const Scalar threshold,
            const Index minDist,
            const Index maxFeatures,
            const bool useSuppresion)
            : FASTDetectorBase<Scalar, Mode, Score>(threshold, minDist, maxFeatures, useSuppresion)
        { }

        /** Compute FAST corner features in the given image.
          * Note that FAST requires a grayscale image as input!
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
                throw std::runtime_error("FAST can only compute single channel images");

            // detect all corners according to the current mode of operation
            std::vector<Vector2i> corners;
            corners.reserve(5000);
            this->detectCorners(img, corners);

            // compute score only if non-maxima suppression is used or we have
            // found more corners than allowed
            Matrix scoreMat;
            if(this->useSuppression_ || (this->maxFeatures_ > 0 && static_cast<Index>(corners.size()) > this->maxFeatures_))
            {
                // compute the cornerness score for each detected corner
                this->score_(img, corners, scoreMat);
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
