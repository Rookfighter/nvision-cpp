/* fast_features.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_FAST_FEATURES_H_
#define CVE_FAST_FEATURES_H_

#include "cve/core/math.h"
#include "cve/core/matrix.h"
#include "cve/core/image.h"

namespace cve
{
    /** FAST corner detection functor. */
    template<typename Scalar>
    class FASTFeatures
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;
    private:
        enum class IntensityClass
        {
            None = 0,
            Lower,
            Higher
        };

        Scalar threshold_;
        Matrixi circle_;
        Index neighs_;
        Index minDist_;
        Index maxFeatures_;

        IntensityClass determineClass(const Scalar value,
            const Scalar low,
            const Scalar high) const
        {
            if(value < low)
                return IntensityClass::Lower;
            if(value > high)
                return IntensityClass::Higher;
            return IntensityClass::None;
        }

        template<typename ScalarA>
        bool isCorner(const Index row, const Index col,
            const Eigen::Tensor<ScalarA, 3> &img) const
        {
            std::vector<IntensityClass> classes(16);

            Scalar low = img(row, col, 0) - threshold_;
            Scalar high = img(row, col, 0) + threshold_;

            Index cnt = 0;
            IntensityClass last = IntensityClass::None;
            for(size_t i = 0; i < classes.size(); ++i)
            {
                Index r2 = row + circle_(1, i);
                Index c2 = col + circle_(0, i);
                if(!image::isInside(r2, c2, img))
                    classes[i] = IntensityClass::None;
                else
                    classes[i] = determineClass(img(r2, c2, 0), low, high);

                if(classes[i] == IntensityClass::None)
                    cnt = 0;
                else if(classes[i] != last)
                    cnt = 1;
                else
                    ++cnt;
                last = classes[i];

                if(cnt >= neighs_)
                    return true;
            }

            if(cnt == 0)
                return false;

            for(Index i = 0; i < neighs_; ++i)
            {
                if(classes[i] == IntensityClass::None || classes[i] != last)
                    return false;
                else
                    ++cnt;
                last = classes[i];
                if(cnt >= neighs_)
                    return true;
            }

            return false;
        }

        template<typename ScalarA>
        Scalar computeScore(const Index row, const Index col,
            const Eigen::Tensor<ScalarA, 3> &img) const
        {
            Scalar score = 0;
            Scalar val = static_cast<Scalar>(img(row, col, 0));
            for(Index i = 0; i < circle_.cols(); ++i)
            {
                Index r2 = row + circle_(1, i);
                Index c2 = col + circle_(0, i);

                if(image::isInside(r2, c2, img))
                {
                    Scalar val2 = static_cast<Scalar>(img(r2, c2, 0));
                    score += std::abs(val - val2);
                }
            }

            return score;
        }
    public:
        FASTFeatures()
            : FASTFeatures(10, 7, 0)
        {

        }

        FASTFeatures(const Scalar threshold,
            const Index minDist,
            const Index maxFeatures)
            : threshold_(threshold), circle_(2, 16), neighs_(12),
            minDist_(minDist), maxFeatures_(maxFeatures)
        {
            circle_ <<  0,  1,  2,  3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1,
                       -3, -3, -2, -1, 0, 1, 2, 3, 3,  3,  2,  1,  0, -1, -2, -3;
        }

        void setThreshold(const Scalar threshold)
        {
            threshold_ = threshold;
        }

        void setMinDistance(const Index minDist)
        {
            minDist_ = minDist;
        }

        void setMaxFeatures(const Index maxFeatures)
        {
            maxFeatures_ = maxFeatures;
        }

        /** Compute FAST corner features in the given image.
          * Note that FAST requires a grayscale image as input!
          * The keypoints are returned in a matrix, where each column
          * represents a single 2D point. Each keypoint is stored in (x y), i.e.
          * (column row), format.
          * Each corner has at least 12 pixels on a circle with a 3 pixel
          * radius, which are signficantly brighter or darker than itself.
          * @param img input grayscale image
          * @param keypoints 2xN matrix with N keypoints
          */
        template<typename ScalarA>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            Matrix &keypoints) const
        {
            if(img.dimension(2) > 1)
                throw std::runtime_error("FAST can only compute single channel images");

            Matrix score(img.dimension(0), img.dimension(1));
            score.setZero();

            std::vector<Vector2i> points1;
            points1.reserve(5000);
            for(Index c = 0; c < img.dimension(1); ++c)
            {
                for(Index r = 0; r < img.dimension(0); ++r)
                {
                    if(isCorner(r, c, img))
                    {
                        points1.push_back({c, r});
                        score(r, c) = computeScore(r, c, img);
                    }
                }
            }

            // perform non-maxima suppression
            std::vector<Vector2i> points2;
            points2.reserve(points1.size());
            for(size_t i = 0; i < points1.size(); ++i)
            {
                Index c = points1[i](0);
                Index r = points1[i](1);
                Scalar val = score(r, c);
                bool best = true;
                for(Index x =  c - minDist_; x < c + minDist_; ++x)
                {
                    for(Index y =  r - minDist_; y < r + minDist_; ++y)
                    {
                        if(image::isInside(y, x, img) && val < score(y, x))
                        {
                            best = false;
                            break;
                        }
                    }

                    if(!best)
                        break;
                }

                if(best)
                    points2.push_back({c, r});
            }

            // check if maximum features has been reached
            if(maxFeatures_ > 0 &&
                static_cast<Index>(points2.size()) > maxFeatures_)
            {
                std::sort(points2.begin(), points2.end(),
                    [&score](const Vector2i &lhs, const Vector2i &rhs)
                    { return score(lhs(1), lhs(0)) < score(rhs(1), rhs(0)); });
            }

            Index featureCnt = points2.size();
            if(maxFeatures_ > 0)
                featureCnt = std::min(maxFeatures_, featureCnt);
            keypoints.resize(2, featureCnt);
            for(Index i = 0; i < keypoints.cols(); ++i)
                keypoints.col(i) << points2[i](0), points2[i](1);
        }
    };
}

#endif
