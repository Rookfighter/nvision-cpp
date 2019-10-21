/* fast_detector_base.h
 *
 * Author: Fabian Meyer
 * Created On: 21 Oct 2019
 */

#ifndef CVE_FAST_DETECTOR_BASE_H_
#define CVE_FAST_DETECTOR_BASE_H_

#include "cve/core/math.h"
#include "cve/core/matrix.h"
#include "cve/core/image.h"
#include "cve/filter/gauss_filter.h"
#include "cve/filter/sobel_filter.h"

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

    template<typename Scalar,
        typename SmoothFilter=GaussFilter<Scalar>,
        typename GradientFilter=SobelFilter<Scalar>>
    class FASTHarrisScore
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, 2, 2> Matrix2;
    private:
        Scalar traceFactor_;
        SmoothFilter smoothFilter_;
        GradientFilter gradientFilter_;
    public:

        FASTHarrisScore()
            : FASTHarrisScore(0.04)
        { }

        FASTHarrisScore(const Scalar traceFactor)
            : traceFactor_(traceFactor), smoothFilter_(), gradientFilter_()
        { }

        void setTraceFactor(const Scalar factor)
        {
            traceFactor_ = factor;
        }

        void setSmoothFilter(const SmoothFilter &filter)
        {
            smoothFilter_ = filter;
        }

        void setGradientFilter(const GradientFilter &filter)
        {
            gradientFilter_ = filter;
        }

        template<typename ScalarA>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
            const std::vector<Vector2i> &corners,
            Matrix &score) const
        {
            Eigen::Tensor<Scalar, 3> gradX;
            Eigen::Tensor<Scalar, 3> gradY;
            Eigen::Tensor<Scalar, 3> gradXX;
            Eigen::Tensor<Scalar, 3> gradYY;
            Eigen::Tensor<Scalar, 3> gradXY;

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
            score.resize(img.dimension(0), img.dimension(1));
            score.setZero();
            for(size_t i = 0; i < corners.size(); ++i)
            {
                Index r = corners[i](1);
                Index c = corners[i](0);

                Matrix2 M;
                M << gradXX(r, c, 0), gradXY(r, c, 0),
                    gradXY(r, c, 0), gradYY(r, c, 0);

                Scalar trace = M(0, 0) + M(1, 1);
                Scalar det = M(0, 0) * M(1, 1) - M(0, 1) * M(1, 0);
                score(r, c) = det - traceFactor_ * trace * trace;
            }
        }
    };

    template<typename Scalar, typename Mode>
    class FASTScore
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
    private:
        Mode mode_;
        Scalar threshold_;

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
            Scalar centerVal = static_cast<Scalar>(img(row, col, 0));
            for(Index i = 0; i < mode_.circle().cols(); ++i)
            {
                Index r2 = row + mode_.circle()(1, i);
                Index c2 = col + mode_.circle()(0, i);

                if(image::isInside(r2, c2, img))
                {
                    Scalar neighVal = static_cast<Scalar>(img(r2, c2, 0));
                    if(neighVal < low)
                        scoreLow += std::abs(centerVal - neighVal);
                    else if(neighVal > high)
                        scoreHigh += std::abs(centerVal - neighVal);
                }
            }

            return std::max(scoreLow, scoreHigh);
        }
    public:
        FASTScore()
            : mode_()
        { }

        template<typename ScalarA>
        void operator()(const Eigen::Tensor<ScalarA, 3> &img,
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
    };

    /** Base for FAST corner detection functors. */
    template<typename Scalar, typename Mode>
    class FASTDetectorBase
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;
    protected:
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
        void detectCorners(const Eigen::Tensor<ScalarA, 3> &img,
            std::vector<Vector2i> &corners) const
        {
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

        void nonMaximaSuppression(const Matrix &score,
            std::vector<Vector2i> &corners) const
        {
            std::vector<Vector2i> tmpCorners;
            tmpCorners.reserve(corners.size());
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
                    tmpCorners.push_back({c, r});
            }

            corners = std::move(tmpCorners);
        }

    public:
        /** Construct a FAST detector base with custom parameters.
          * @param threshold FAST threshold which determines if a pixel is brighter or darker
          * @param minDist minimum distance between keypoints in non-maxima suppression
          * @param maxFeatures maximum number of features that should be detected
          * @param useSuppresion determine if non-maxima suppression is used */
        FASTDetectorBase(const Scalar threshold,
            const Index minDist,
            const Index maxFeatures,
            const bool useSuppresion)
            : mode_(), threshold_(threshold), minDist_(minDist),
            maxFeatures_(maxFeatures), useSuppression_(useSuppresion)
        { }

        /** Set the threshold after which a pixel is considered brighter / darker
          * than its corresponding keypoint.
          * A pixel is considered brighter / darker than its corresponding keypoint if:
          *
          * pixval < kpval - threshold
          *
          * pixval > kpval + threshold
          *
          * @param threshold threshold for pixel comparison
          */
        void setThreshold(const Scalar threshold)
        {
            threshold_ = threshold;
        }

        /** Set the minimum distance between two keypoints for non-maxima
          * suppression.
          * @param minDist minimum distance between keypoints */
        void setMinDistance(const Index minDist)
        {
            minDist_ = minDist;
        }

        /** Set the maximum number of features, which should be detected.
          * If more features are found, the most suitable features are selected
          * according to a cornerness score.
          * @param maxFeatures maximum number of features */
        void setMaxFeatures(const Index maxFeatures)
        {
            maxFeatures_ = maxFeatures;
        }

        /** En- or disable non-maxima suppression.
          * @param useSuppresion set true to enable non-maxima suppression */
        void setNonMaximaSuppression(const bool useSuppression)
        {
            useSuppression_ = useSuppression;
        }
    };
}

#endif
