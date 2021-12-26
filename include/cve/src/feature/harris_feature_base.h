/* harris_feature_base.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#ifndef CVE_HARRIS_FEATURE_BASE_H_
#define CVE_HARRIS_FEATURE_BASE_H_

#include "cve/src/core/image.h"
#include "cve/src/filter/gauss_filter.h"
#include "cve/src/filter/sobel_filter.h"

namespace cve
{
    /** Base class for Harris corner detection algorithms,
      * which can be parametrized over its score function. */
    template<typename _Scalar,
             typename _Score,
             typename _SmoothFilter=GaussFilter<_Scalar, 3>,
             typename _GradientFilter=SobelFilter<_Scalar>>
    class HarrisFeatureBase
    {
    public:
        using Scalar = _Scalar;
        using Score = _Score;
        using SmoothFilter = _SmoothFilter;
        using GradientFilter = _GradientFilter;
        using FeatureMatrix = Eigen::Matrix<Scalar, 2, Eigen::Dynamic>;
        using Matrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;

        HarrisFeatureBase() = default;

        HarrisFeatureBase(const Scalar qualityLevel,
            const Index minDistance,
            const Index maxCorners)
            : _qualityLevel(qualityLevel), _minDistance(minDistance), _maxCorners(maxCorners)
        { }

        void setSmoothFilter(const SmoothFilter &smooth)
        {
            _smooth = smooth;
        }

        void setGradientFilter(const GradientFilter &gradient)
        {
            _gradient = gradient;
        }

        /** Sets the relative quality level when corners are accepted.
          * The value has to be in the range [0,1].
          * @param qualityLevel quality level */
        void setQualityLevel(const Scalar qualityLevel)
        {
            assert(qualityLevel >= static_cast<Scalar>(0) && qualityLevel <= static_cast<Scalar>(1));
            _qualityLevel = cve::clamp<Scalar>(qualityLevel, 0, 1);
        }

        /** Sets the maximum amount of corners which can be detected.
          * If more corners are detected, only the ones with highest harris
          * corner score are returned.
          * @param maxCorners maximum number of corners that can be detected. */
        void setMaximumCorners(const Index maxCorners)
        {
            _maxCorners = maxCorners;
        }

        /** Set the minimum distance in pixels that the features have to be
          * apart
          * @param distance minimum distance between features. */
        void setMinDistance(const Index distance)
        {
            _minDistance = distance;
        }

        /** Compute harris corner features in the given image.
          * The keypoints are returned in a matrix, where each column
          * represents a single 2D point. Each keypoint is stored in (x y), i.e.
          * (column, row) format.
          * The keypoints are stored in descending order according to their
          * quality measure.
          * @param img input image
          * @param keypoints 2xN matrix with N keypoints */
        template<typename ColorSpace>
        void operator()(const Image<ColorSpace> &img,
                        FeatureMatrix &keypoints) const
        {
            using Matrix2 = Eigen::Matrix<Scalar, 2, 2>;
            using Vector2i = Eigen::Matrix<Index, 2, 1>;

            // calculate base gradients
            Image<ColorSpace> gradX = _gradient(img, GradientMode::X());
            Image<ColorSpace> gradY = _gradient(img, GradientMode::Y());

            // accumulate values from local neighbourhood with smooth filter
            Image<ColorSpace> tmp = gradX * gradX;
            Image<ColorSpace> gradXX = _smooth(tmp);

            tmp = gradY * gradY;
            Image<ColorSpace> gradYY = _smooth(tmp);

            tmp = gradX * gradY;
            Image<ColorSpace> gradXY = _smooth(tmp);

            Matrix response(img.rows(), img.cols());

            // compute the harris response
            for(Index c = 0; c < gradXX.cols(); ++c)
            {
                for(Index r = 0; r < gradXX.rows(); ++r)
                {
                    Matrix2 M = Matrix2::Zero();

                    for(Index d = 0; d < ColorSpace::Dimension; ++d)
                    {
                        M(0, 0) += gradXX(r, c)[d];
                        M(0, 1) += gradXY(r, c)[d];
                        M(1, 0) += gradXY(r, c)[d];
                        M(1, 1) += gradYY(r, c)[d];
                    }

                    M /= ColorSpace::Dimension;

                    response(r, c) = _score(M);
                }
            }

            const auto maxResponse = response.maxCoeff();

            // count all corners for efficient preallocation
            Index cnt = 0;
            for(Index c = 0; c < response.cols(); ++c)
            {
                for(Index r = 0; r < response.rows(); ++r)
                {
                    if(isCorner(response, r, c, maxResponse))
                        ++cnt;
                }
            }

            std::vector<Vector2i> pointsTmp(cnt);

            cnt = 0;
            for(Index c = 0; c < response.cols(); ++c)
            {
                for(Index r = 0; r < response.rows(); ++r)
                {
                    if(isCorner(response, r, c, maxResponse))
                    {
                        pointsTmp[cnt] << c, r;
                        ++cnt;
                    }
                }
            }

            std::sort(pointsTmp.begin(), pointsTmp.end(),
                [&response](const auto &v1, const auto &v2)
                { return response(v1(1), v1(0)) < response(v2(1), v2(0)); }
            );

            if(_maxCorners > 0)
                cnt = std::min(_maxCorners, static_cast<Index>(pointsTmp.size()));
            else
                cnt = static_cast<Index>(pointsTmp.size());

            keypoints.resize(2, cnt);
            for(Index i = 0; i < cnt; ++i)
            {
                keypoints.col(i) << static_cast<Scalar>(pointsTmp[i].x()), static_cast<Scalar>(pointsTmp[i].y());
            }
        }

    private:

        Scalar _qualityLevel = static_cast<Scalar>(0.1);
        Index _minDistance = 10;
        Index _maxCorners = 0;

        SmoothFilter _smooth = {};
        GradientFilter _gradient = {};
        Score _score = {};

        /** Determines if given pixel is a corner.
          * A pixel is considered a suitable corner if:
          *   - its response is greater than qualityLevel * maximum(response)
          *   - it has the maximum response in its local neighbourhood */
        bool isCorner(const Matrix &response,
                      const Index row,
                      const Index col,
                      const Scalar maxResponse) const
        {
            const auto val = response(row, col);
            // check if corner value is high enough to be considered
            if(val < _qualityLevel * maxResponse)
                return false;

            bool allSame = true;
            for(Index x = -_minDistance; x < _minDistance+1; ++x)
            {
                const auto c2 = col + x;
                if(c2 < 0 || c2 >= response.cols())
                    continue;

                for(Index y = -_minDistance; y < _minDistance+1; ++y)
                {
                    const auto r2 = row + y;
                    if(r2 < 0 || r2 >= response.rows())
                        continue;

                    if(val < response(r2, c2))
                        return false;
                    if(val != response(r2, c2))
                        allSame = false;
                }
            }

            return !allSame;
        }
    };
}

#endif
