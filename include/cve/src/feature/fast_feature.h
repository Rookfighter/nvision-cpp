/* fast_feature.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_FAST_FEATURE_H_
#define CVE_FAST_FEATURE_H_

#include "cve/src/feature/fast_feature_base.h"

namespace cve
{
    /** FAST feature detection functor.
      *
      * This corner detector essentially checks the pixels around a potential
      * keypoint to be either lighter or darker than the keypoint itself.
      * If the pixels on the circle form a long enough sequence, the keypoint
      * is accepted. */
    template<typename _Scalar, typename _Mode=FASTMode7, typename _Score=FASTScore<_Scalar, _Mode>>
    class FASTFeature : public FASTFeatureBase<_Scalar, _Mode, _Score>
    {
    public:
        using Scalar = _Scalar;
        using Mode = _Mode;
        using Score = _Score;
        using Matrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
        using FeatureMatrix = Eigen::Matrix<Scalar, 2, Eigen::Dynamic>;
        using Vector2i = Eigen::Matrix<Index, 2, 1>;

        static_assert(Eigen::NumTraits<Scalar>::IsInteger == 0, "Scalar must be floating point");

        FASTFeature()
            : FASTFeature(static_cast<Scalar>(0.08), 16, 0, true)
        { }

        /** Construct a FAST detector with custom parameters.
          * @param threshold FAST threshold which determines if a pixel is brighter or darker
          * @param minDist minimum distance between keypoints in non-maxima suppression
          * @param maxFeatures maximum number of features that should be detected
          * @param useSuppresion determine if non-maxima suppression is used */
        FASTFeature(const Scalar threshold,
                    const Index minDist,
                    const Index maxFeatures,
                    const bool useSuppresion)
            : FASTFeatureBase<Scalar, Mode, Score>(threshold, minDist, maxFeatures, useSuppresion)
        { }

        ~FASTFeature()
        { }

        /** Compute FAST corner features in the given image.
          * Note that FAST requires a grayscale image as input!
          * The keypoints are returned in a matrix, where each column
          * represents a single 2D point. Each keypoint is stored in (x y), i.e.
          * (column row), format.
          * @param img input grayscale image
          * @param keypoints 2xN matrix with N keypoints */
        template<typename ColorSpace>
        void operator()(const Image<ColorSpace> &img,
                        FeatureMatrix &keypoints) const
        {
            static_assert(ColorSpace::Dimension == 1, "FAST only supports single channel images");

            // detect all corners according to the current mode of operation
            std::vector<Vector2i> corners;
            corners.reserve(5000);
            this->detectCorners(img, corners);

            // compute score only if non-maxima suppression is used or we have
            // found more corners than allowed
            Matrix scoreMat;
            if(this->useNonMaximaSuppression() || (this->maxFeatures() > 0 && static_cast<Index>(corners.size()) > this->maxFeatures()))
            {
                // compute the cornerness score for each detected corner
                this->computeScore(img, corners, scoreMat);
            }

            if(this->useNonMaximaSuppression())
            {
                // perform non-maxima suppression to make features sparser
                // basically maintain a minimum distance between feature points
                this->nonMaximaSuppression(scoreMat, corners);
            }

            auto featureCnt = static_cast<Index>(corners.size());
            if(this->maxFeatures() > 0)
                featureCnt = std::min(this->maxFeatures(), featureCnt);

            // check if maximum features has been reached
            if(featureCnt != static_cast<Index>(corners.size()))
            {
                std::sort(corners.begin(), corners.end(),
                    [&scoreMat](const auto &lhs, const auto &rhs)
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
