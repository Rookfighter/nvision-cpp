/* fast_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_FAST_DETECTOR_H_
#define CVE_FAST_DETECTOR_H_

#include "cve/core/math.h"
#include "cve/core/matrix.h"
#include "cve/core/image.h"

namespace cve
{
    /** Class to let FAST operate on a circle with diameter of 5 pixels.
      * The sequence length has be to be 9 to accept a pixel as corner. */
    class FASTMode5
    {
    private:
        Matrixi circle_;
        Index sequence_;
    public:
        FASTMode5()
            : circle_(2, 12), sequence_(9)
        {
            circle_ <<  0,  1,  2, 2, 2, 1, 0, -1, -2, -2, -2, -1,
                       -2, -2, -1, 0, 1, 2, 2,  2,  1,  0, -1, -2;
        }

        Index sequence() const
        {
            return sequence_;
        }

        const Matrixi &circle() const
        {
            return circle_;
        }
    };

    /** Class to let FAST operate on a circle with diameter of 7 pixels.
      * The sequence length has be to be 12 to accept a pixel as corner. */
    class FASTMode7
    {
    private:
        Matrixi circle_;
        Index sequence_;
    public:
        FASTMode7()
            : circle_(2, 16), sequence_(12)
        {
            circle_ <<  0,  1,  2,  3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1,
                       -3, -3, -2, -1, 0, 1, 2, 3, 3,  3,  2,  1,  0, -1, -2, -3;
        }

        Index sequence() const
        {
            return sequence_;
        }

        const Matrixi &circle() const
        {
            return circle_;
        }
    };

    /** Class to let FAST operate on a circle with diameter of 9 pixels.
      * The sequence length has be to be 15 to accept a pixel as corner. */
    class FASTMode9
    {
    private:
        Matrixi circle_;
        Index sequence_;
    public:
        FASTMode9()
            : circle_(2, 20), sequence_(15)
        {
            circle_ <<  0,  1,  2,  3,  4, 4, 4, 3, 2, 1, 0, -1, -2, -3, -4, -4, -4, -3, -2, -1,
                       -4, -4, -3, -2, -1, 0, 1, 2, 3, 4, 4,  4,  3,  2,  1,  0, -1, -2, -3, -4;
        }

        Index sequence() const
        {
            return sequence_;
        }

        const Matrixi &circle() const
        {
            return circle_;
        }
    };

    /** FAST corner detection functor. */
    template<typename Scalar, typename Mode=FASTMode7>
    class FASTDetector
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;
    private:
        Mode mode_;
        Scalar threshold_;;
        Index minDist_;
        Index maxFeatures_;
        bool useSuppression_;

        enum class IntensityClass
        {
            None = 0,
            Lower,
            Higher
        };

        template<typename ScalarA>
        IntensityClass determineClass(const Eigen::Tensor<ScalarA, 3> &img,
            const Index row,
            const Index col,
            const Scalar low,
            const Scalar high) const
        {
            if(!image::isInside(row, col, img))
                return IntensityClass::None;

            Scalar value = static_cast<Scalar>(img(row, col, 0));
            if(value < low)
                return IntensityClass::Lower;
            else if(value > high)
                return IntensityClass::Higher;
            else
                return IntensityClass::None;
        }

        template<typename ScalarA>
        bool fastCornerTest(const Eigen::Tensor<ScalarA, 3> &img,
            const Index row, const Index col) const
        {
            // determine lower intensity boundary
            Scalar low = img(row, col, 0) - threshold_;
            // determine upper intensity boundary
            Scalar high = img(row, col, 0) + threshold_;

            Index quart = mode_.circle().cols() / 4;

            IntensityClass north = determineClass(img, row + mode_.circle()(1, 0),       col + mode_.circle()(0, 0), low, high);
            IntensityClass east  = determineClass(img, row + mode_.circle()(1, quart),   col + mode_.circle()(0, quart), low, high);
            IntensityClass south = determineClass(img, row + mode_.circle()(1, 2*quart), col + mode_.circle()(0, 2*quart), low, high);
            IntensityClass west  = determineClass(img, row + mode_.circle()(1, 3*quart), col + mode_.circle()(0, 3*quart), low, high);

            bool testA = north == east && east == south;
            bool testB = east == south && south == west;
            bool testC = south == west && west == north;

            return testA || testB || testC;
        }

        template<typename ScalarA>
        bool fullCornerTest(const Eigen::Tensor<ScalarA, 3> &img,
            const Index row, const Index col) const
        {
            // determine lower intensity boundary
            Scalar low = img(row, col, 0) - threshold_;
            // determine upper intensity boundary
            Scalar high = img(row, col, 0) + threshold_;

            // count the length of sequence of pixels which have same intensity
            // class
            Index cnt = 0;
            // always keep track of the previous intesity class
            IntensityClass prev = IntensityClass::None;
            for(Index i = 0; i < mode_.circle().cols(); ++i)
            {
                // compute pixel coordinates based on given circle
                Index r2 = row + mode_.circle()(1, i);
                Index c2 = col + mode_.circle()(0, i);
                // determine the intensity class of this pixel
                IntensityClass curr = determineClass(img, r2, c2, low, high);

                // if it has no intensity class, start over with counting
                // (e.g. out of bounds or between low and high)
                if(curr == IntensityClass::None)
                    cnt = 0;
                // if the current is not the same class as the previous one
                // start a new sequence
                else if(curr != prev)
                    cnt = 1;
                // if last and current have the same class then increment
                // sequence length
                else
                    ++cnt;
                prev = curr;

                // if the current sequence is long enough simply return true at
                // this point
                if(cnt >= mode_.sequence())
                    return true;
            }

            // if sequence length is zero there was no corner found
            if(cnt == 0)
                return false;

            for(Index i = 0; i <  mode_.sequence(); ++i)
            {
                // compute pixel coordinates based on given circle
                Index r2 = row + mode_.circle()(1, i);
                Index c2 = col + mode_.circle()(0, i);
                // determine the intensity class of this pixel
                IntensityClass curr = determineClass(img, r2, c2, low, high);

                // if current pixel has no intensity class or it stops the
                // sequence, the return false, no corner detected
                if(curr == IntensityClass::None || curr != prev)
                    return false;
                else
                    ++cnt;
                prev = curr;

                if(cnt >= mode_.sequence())
                    return true;
            }

            return false;
        }

        template<typename ScalarA>
        Scalar computeScore(const Eigen::Tensor<ScalarA, 3> &img,
            const Index row, const Index col) const
        {
            // determine lower intensity boundary
            Scalar low = img(row, col, 0) - threshold_;
            // determine upper intensity boundary
            Scalar high = img(row, col, 0) + threshold_;

            Scalar scoreLow = 0;
            Scalar scoreHigh = 0;
            Scalar val = static_cast<Scalar>(img(row, col, 0));
            for(Index i = 0; i < mode_.circle().cols(); ++i)
            {
                Index r2 = row + mode_.circle()(1, i);
                Index c2 = col + mode_.circle()(0, i);

                IntensityClass curr = determineClass(img, r2, c2, low, high);

                if(curr == IntensityClass::Lower)
                    scoreLow += std::abs(val - static_cast<Scalar>(img(r2, c2, 0)));
                else if(curr == IntensityClass::Higher)
                    scoreHigh += std::abs(val - static_cast<Scalar>(img(r2, c2, 0)));
            }

            return std::max(scoreLow, scoreHigh);
        }

        template<typename ScalarA>
        void detectCorners(const Eigen::Tensor<ScalarA, 3> &img,
            std::vector<Vector2i> &corners) const
        {
            corners.reserve(5000);
            for(Index c = 0; c < img.dimension(1); ++c)
            {
                for(Index r = 0; r < img.dimension(0); ++r)
                {
                    if(fastCornerTest(img, r, c) && fullCornerTest(img, r, c))
                        corners.push_back({c, r});
                }
            }
        }

        template<typename ScalarA>
        void computeScoreMatrix(const Eigen::Tensor<ScalarA, 3> &img,
            const std::vector<Vector2i> &corners,
            Matrix &score) const
        {
            score.resize(img.dimension(0), img.dimension(1));
            score.setZero();

            for(size_t i = 0; i < corners.size(); ++i)
            {
                Index r = corners[i](1);
                Index c = corners[i](0);
                score(r, c) = computeScore(img, r, c);
            }
        }

        void nonMaximaSuppression(const std::vector<Vector2i> &corners,
            const Matrix &score,
            std::vector<Vector2i> &outCorners) const
        {
            outCorners.reserve(corners.size());
            for(size_t i = 0; i < corners.size(); ++i)
            {
                Index c = corners[i](0);
                Index r = corners[i](1);
                Scalar val = score(r, c);

                bool best = true;
                for(Index x =  c - minDist_; x < c + minDist_; ++x)
                {
                    for(Index y =  r - minDist_; y < r + minDist_; ++y)
                    {
                        if(y >= 0 && y < score.rows()
                            && x >= 0 && x < score.cols()
                            && val < score(y, x))
                        {
                            best = false;
                            break;
                        }
                    }

                    if(!best)
                        break;
                }

                if(best)
                    outCorners.push_back({c, r});
            }
        }

    public:
        FASTDetector()
            : FASTDetector(10, 7, 0, true)
        {

        }

        FASTDetector(const Scalar threshold,
            const Index minDist,
            const Index maxFeatures,
            const bool useSuppresion)
            : mode_(), threshold_(threshold), minDist_(minDist),
            maxFeatures_(maxFeatures), useSuppression_(useSuppresion)
        {

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

        void setNonMaximaSuppression(const bool useSuppression)
        {
            useSuppression_ = useSuppression;
        }

        /** Compute FAST corner features in the given image.
          * Note that FAST requires a grayscale image as input!
          * The keypoints are returned in a matrix, where each column
          * represents a single 2D point. Each keypoint is stored in (x y), i.e.
          * (column row), format.
          * @param img input grayscale image
          * @param keypoints 2xN matrix with N keypoints
          */
        template<typename ScalarA>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            Matrix &keypoints) const
        {
            if(img.dimension(2) > 1)
                throw std::runtime_error("FAST can only compute single channel images");

            // detect all corners according to the current mode of operation
            std::vector<Vector2i> corners;
            detectCorners(img, corners);

            // computer score only if non-maxima suppression is used or we have
            // found more corners than allowed
            Matrix score;
            if(useSuppression_ || (maxFeatures_ > 0 && static_cast<Index>(corners.size()) > maxFeatures_))
            {
                // compute the cornerness score for each detected corner
                computeScoreMatrix(img, corners, score);
            }

            if(useSuppression_)
            {
                // perform non-maxima suppression to make features sparser
                // basically maintain a minimum distance between feature points
                std::vector<Vector2i> tmp;
                nonMaximaSuppression(corners, score, tmp);
                corners = std::move(tmp);
            }

            Index featureCnt = static_cast<Index>(corners.size());
            if(maxFeatures_ > 0)
                featureCnt = std::min(maxFeatures_, featureCnt);

            // check if maximum features has been reached
            if(featureCnt != static_cast<Index>(corners.size()))
            {
                std::sort(corners.begin(), corners.end(),
                    [&score](const Vector2i &lhs, const Vector2i &rhs)
                    { return score(lhs(1), lhs(0)) < score(rhs(1), rhs(0)); });
            }

            // copy detected points into output matrix
            keypoints.resize(2, featureCnt);
            for(Index i = 0; i < keypoints.cols(); ++i)
                keypoints.col(i) = corners[i].template cast<Scalar>();
        }
    };
}

#endif
