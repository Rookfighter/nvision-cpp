/* orb_feature.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef NVISION_ORB_FEATURE_H_
#define NVISION_ORB_FEATURE_H_

#include "nvision/src/feature/fast_feature_base.h"

namespace nvision
{
    /** ORB feature detection functor.
      *
      * This detector basically performs FAST feature detection on a scaled
      * image pyramid.
      * After each feature detection step it scales the image by a given factor
      * and performs another FAST detection. */
    template<typename _Scalar, typename _Mode=FASTMode9, typename _Score=FASTHarrisScore<_Scalar>>
    class ORBFeature : public FASTFeatureBase<_Scalar, _Mode, _Score>
    {
    public:
        using Scalar = _Scalar;
        using Mode = _Mode;
        using Score = _Score;
        using FeatureMatrix = Eigen::Matrix<Scalar, 2, Eigen::Dynamic>;
        using Matrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
        using Vector2i = Eigen::Matrix<Index, 2, 1>;

        static_assert(Eigen::NumTraits<Scalar>::IsInteger == 0, "Scalar must be floating point");

        ORBFeature()
            : ORBFeature(static_cast<Scalar>(0.08), 16, 0, true)
        { }

        /** Construct an ORB detector with custom parameters.
          * @param threshold FAST threshold which determines if a pixel is brighter or darker
          * @param minDist minimum distance between keypoints in non-maxima suppression
          * @param maxFeatures maximum number of features that should be detected
          * @param useSuppression determine if non-maxima suppression is used
          * @param pyramidFactor scaling factor of the images in the ORB pyramid
          * @param pyramidLevels number of ORB pyramid levels */
        ORBFeature(const Scalar threshold,
            const Index minDist,
            const Index maxFeatures,
            const bool useSuppression)
            : FASTFeatureBase<Scalar, Mode, Score>(threshold, minDist, maxFeatures, useSuppression)
        { }

        ~ORBFeature()
        { }

        /** Compute ORB corner features in the given image.
          * Note that ORB requires a grayscale image as input!
          * The keypoints are returned in a matrix, where each column
          * represents a single 2D point. Each keypoint is stored in (x y), i.e.
          * (column row), format.
          * @param img input grayscale image
          * @param keypoints 2xN matrix with N keypoints
          * @param levels number of levels for the image pyramid
          * @param factor uniform scaling factor for image pyramid */
        template<typename ColorSpace>
        void operator()(const Image<ColorSpace> &img,
                        FeatureMatrix &keypoints,
                        const Index levels = 8,
                        const Scalar factor = static_cast<Scalar>(0.8)) const
        {
            static_assert(ColorSpace::Dimension == 1, "ORB only supports single channel images");

            const auto pyramid = image::makePyramid(img, levels, factor);
            this->operator()(pyramid, keypoints);
        }

        /** Compute ORB corner features in the given image.
          * Note that ORB requires a grayscale image as input!
          * The keypoints are returned in a matrix, where each column
          * represents a single 2D point. Each keypoint is stored in (x y), i.e.
          * (column row), format.
          * @param pyramid input image pyramid
          * @param keypoints 2xN matrix with N keypoints */
        template<typename ColorSpace>
        void operator()(const ImagePyramid<Scalar, ColorSpace> &pyramid,
                        FeatureMatrix &keypoints) const
        {
            static_assert(ColorSpace::Dimension == 1, "ORB only supports single channel images");

            const auto &img = pyramid.images().front();

            std::vector<Vector2i> corners;
            corners.reserve(5000 * pyramid.size());

            // perform FAST detection for each level of the pyramid
            for(Index i = 0; i < pyramid.size(); ++i)
            {
                const auto &pimg = pyramid.images()[i];
                const auto &level = pyramid.levels()[i];
                const auto sizeBefore = corners.size();

                // detect all corners according to the current mode of operation
                this->detectCorners(pimg, corners);

                // scale corners back to original image positions
                for(size_t j = sizeBefore; j < corners.size(); ++j)
                {
                    auto &corner = corners[j];
                    corner(0) = static_cast<Scalar>(corner(0)) / level.scaleX;
                    corner(1) = static_cast<Scalar>(corner(1)) / level.scaleY;
                }
            }

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
