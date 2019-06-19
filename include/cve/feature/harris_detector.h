/* harris_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#ifndef CVE_HARRIS_DETECTOR_H_
#define CVE_HARRIS_DETECTOR_H_

#include "cve/math.h"
#include "cve/image.h"
#include "cve/filter/gauss_filter.h"
#include "cve/filter/sobel_filter.h"

namespace cve
{
    /** Class for harris corner detection. */
    template<typename Scalar,
        typename SmoothFilter=GaussFilter<Scalar, 5>,
        typename GradientFilter=SobelFilter<Scalar>>
    class HarrisDetector
    {
    private:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, 2, 2> Matrix2;
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;

        Scalar traceFactor_;
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
                }
            }

            return true;
        }

    public:
        HarrisDetector()
            : HarrisDetector(0.04, 0.01, 10, 0)
        {

        }

        HarrisDetector(const Scalar traceFactor,
            const Scalar qualityLevel,
            const size_t minDistance,
            const size_t maxCorners)
            : traceFactor_(traceFactor), qualityLevel_(qualityLevel),
            minDistance_(minDistance), maxCorners_(maxCorners), smoothFilter_(), gradientFilter_()
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

        void setTraceFactor(const Scalar traceFactor)
        {
            traceFactor_ = traceFactor;
        }

        void setQualityLevel(const Scalar level)
        {
            qualityLevel_ = level;
        }

        void setMaximumCorners(const size_t cnt)
        {
            maxCorners_ = cnt;
        }

        void setMinDistance(const size_t distance)
        {
            minDistance_ = distance;
        }


        /** Compute Harris corner features in the given image.
          * The keypoints are returned in a matrix, where each column
          * represents a single 2D point. Each keypoint is stored in (x y), i.e.
          * (column row), format.
          * The keypoints are stored in descending order according to their
          * quality measure.
          * @param srcImg input image
          * @param keypoints 2xN matrix with N keypoints
          */
        template<typename ScalarA>
        void apply(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Matrix &keypoints) const
        {
            Eigen::Tensor<Scalar, 3> gradX(srcImg.dimensions());
            Eigen::Tensor<Scalar, 3> gradY(srcImg.dimensions());
            Eigen::Tensor<Scalar, 3> gradXX(srcImg.dimensions());
            Eigen::Tensor<Scalar, 3> gradYY(srcImg.dimensions());
            Eigen::Tensor<Scalar, 3> gradXY(srcImg.dimensions());
            Eigen::Tensor<Scalar, 3> response(srcImg.dimension(0), srcImg.dimension(1), 1);

            // calculate gradients
            gradientFilter_.applyX(srcImg, gradX);
            gradientFilter_.applyY(srcImg, gradY);

            // calculate gradients magnitudes
            gradXX = gradX * gradX;
            gradYY = gradY * gradY;
            gradXY = gradX * gradY;

            // accumulate values from local neighbourhood with smooth filter
            smoothFilter_.apply(gradXX);
            smoothFilter_.apply(gradYY);
            smoothFilter_.apply(gradXY);

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

                    Scalar trace = M(0, 0) + M(1, 1);
                    Scalar det = M(0, 0) * M(1, 1) - M(0, 1) * M(1, 0);
                    response(r, c, 0) = det - traceFactor_ * trace * trace;
                }
            }

            // calculate maximum response value
            Scalar maxResponse = 0;
            for(Index c = 0; c < response.dimension(1); ++c)
                for(Index r = 0; r < response.dimension(0); ++r)
                    maxResponse = std::max(maxResponse, response(r, c, 0));

            std::cout << "Max response:" << maxResponse << std::endl;

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
