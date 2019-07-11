/* harris_base_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#ifndef CVE_HARRIS_BASE_DETECTOR_H_
#define CVE_HARRIS_BASE_DETECTOR_H_

#include "cve/core/math.h"
#include "cve/core/image.h"
#include "cve/filter/gauss_filter.h"
#include "cve/filter/sobel_filter.h"

namespace cve
{
    /** Class for harris corner detection. */
    template<typename Scalar,
        typename SmoothFilter=GaussFilter<Scalar>,
        typename GradientFilter=SobelFilter<Scalar>>
    class HarrisBaseDetector
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    protected:
        typedef Eigen::Matrix<Scalar, 2, 2> Matrix2;
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;

        Scalar qualityLevel_;
        Index minDistance_;
        Index maxCorners_;

        SmoothFilter smoothFilter_;
        GradientFilter gradientFilter_;

        /** Determines if given pixel is a corner.
          * A pixel is considered a suitable corner if:
          *   - its response is greater than qualityLevel * maximum(response)
          *   - it has the maximum response in its local neighbourhood */
        bool isCorner(const Eigen::Tensor<Scalar, 3> &response,
            const Index row,
            const Index col,
            const Scalar maxResponse) const
        {
            Scalar val = response(row, col, 0);
            // check if corner value is high enough to be considered
            if(val < qualityLevel_ * maxResponse)
                return false;

            bool allSame = true;
            for(Index x = -minDistance_; x < minDistance_+1; ++x)
            {
                Index c2 = col + x;
                if(c2 < 0 || c2 >= response.dimension(1))
                    continue;

                for(Index y = -minDistance_; y < minDistance_+1; ++y)
                {
                    Index r2 = row + y;
                    if(r2 < 0 || r2 >= response.dimension(0))
                        continue;

                    if(val < response(r2, c2, 0))
                        return false;
                    if(val != response(r2, c2, 0))
                        allSame = false;
                }
            }

            return !allSame;
        }

        virtual Scalar computeScore(const Matrix2 &M) const = 0;

    public:
        HarrisBaseDetector()
            : HarrisBaseDetector(0.01, 10, 0)
        {

        }

        HarrisBaseDetector(const Scalar qualityLevel,
            const size_t minDistance,
            const size_t maxCorners)
            : qualityLevel_(qualityLevel), minDistance_(minDistance),
            maxCorners_(maxCorners), smoothFilter_(), gradientFilter_()
        {

        }

        virtual ~HarrisBaseDetector()
        {

        }

        void setSmoothFilter(const SmoothFilter &filter)
        {
            smoothFilter_ = filter;
        }

        void setGradientFilter(const GradientFilter &gradient)
        {
            gradientFilter_ = gradient;
        }

        /** Sets the relative quality level when corners are accepted.
          * The value has to be in the range [0,1].
          * @param qualityLevel quality level */
        void setQualityLevel(const Scalar qualityLevel)
        {
            qualityLevel_ = std::max<Scalar>(0, std::min<Scalar>(1,
                qualityLevel));
        }

        /** Sets the maximum amount of corners which can be detected.
          * If more corners are detected, only the ones with highest harris
          * corner score are returned.
          * @param maxCorners maximum number of corners that can be detected. */
        void setMaximumCorners(const size_t maxCorners)
        {
            maxCorners_ = maxCorners;
        }

        /** Set the minimum distance in pixels that the features have to be
          * apart
          * @param distance minimum distance between features. */
        void setMinDistance(const size_t distance)
        {
            minDistance_ = distance;
        }


        /** Compute corner features in the given image.
          * The keypoints are returned in a matrix, where each column
          * represents a single 2D point. Each keypoint is stored in (x y), i.e.
          * (column row), format.
          * The keypoints are stored in descending order according to their
          * quality measure.
          * @param img input image
          * @param keypoints 2xN matrix with N keypoints
          */
        template<typename ScalarA>
        void detect(const Eigen::Tensor<ScalarA, 3> &img,
            Matrix &keypoints) const
        {
            Eigen::Tensor<Scalar, 3> gradX(img.dimensions());
            Eigen::Tensor<Scalar, 3> gradY(img.dimensions());
            Eigen::Tensor<Scalar, 3> gradXX(img.dimensions());
            Eigen::Tensor<Scalar, 3> gradYY(img.dimensions());
            Eigen::Tensor<Scalar, 3> gradXY(img.dimensions());
            Eigen::Tensor<Scalar, 3> response(img.dimension(0), img.dimension(1), 1);

            // calculate gradients
            gradientFilter_(img, gradX, gradY);

            // calculate gradients magnitudes
            gradXX = gradX * gradX;
            gradYY = gradY * gradY;
            gradXY = gradX * gradY;

            // accumulate values from local neighbourhood with smooth filter
            smoothFilter_(gradXX);
            smoothFilter_(gradYY);
            smoothFilter_(gradXY);

            // compute the harris response
            for(Index c = 0; c < gradXX.dimension(1); ++c)
            {
                for(Index r = 0; r < gradXX.dimension(0); ++r)
                {
                    Matrix2 M;
                    M.setZero();

                    for(Index d = 0; d < gradXX.dimension(2); ++d)
                    {
                        M(0, 0) += gradXX(r, c, d);
                        M(0, 1) += gradXY(r, c, d);
                        M(1, 0) += gradXY(r, c, d);
                        M(1, 1) += gradYY(r, c, d);
                    }
                    M /= gradXX.dimension(2);

                    response(r, c, 0) = computeScore(M);
                }
            }

            // calculate maximum response value
            Scalar maxResponse = 0;
            for(Index c = 0; c < response.dimension(1); ++c)
                for(Index r = 0; r < response.dimension(0); ++r)
                    maxResponse = std::max(maxResponse, response(r, c, 0));

            Index cnt = 0;
            for(Index c = 0; c < response.dimension(1); ++c)
                for(Index r = 0; r < response.dimension(0); ++r)
                    if(isCorner(response, r, c, maxResponse))
                        ++cnt;

            std::vector<Vector2> pointsTmp(cnt);

            cnt = 0;
            for(Index c = 0; c < response.dimension(1); ++c)
            {
                for(Index r = 0; r < response.dimension(0); ++r)
                {
                    if(isCorner(response, r, c, maxResponse))
                    {
                        pointsTmp[cnt] << c, r;
                        ++cnt;
                    }
                }
            }

            std::sort(pointsTmp.begin(), pointsTmp.end(),
                [&response](const Vector2 &v1, const Vector2 &v2)
                { return response(v1(1), v1(0), 0) < response(v2(1), v2(0), 0); }
            );

            if(maxCorners_ > 0)
                cnt = std::min(maxCorners_, static_cast<Index>(pointsTmp.size()));
            else
                cnt = pointsTmp.size();
            keypoints.resize(2, cnt);
            for(Index i = 0; i < cnt; ++i)
                keypoints.col(i) = pointsTmp[i];
        }
    };
}

#endif
