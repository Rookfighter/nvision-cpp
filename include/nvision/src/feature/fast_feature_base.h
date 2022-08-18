/* fast_feature_base.h
 *
 * Author: Fabian Meyer
 * Created On: 21 Oct 2019
 */

#ifndef NVISION_FAST_FEATURE_BASE_H_
#define NVISION_FAST_FEATURE_BASE_H_

#include "nvision/src/core/image.h"
#include "nvision/src/filter/gauss_filter.h"
#include "nvision/src/filter/sobel_filter.h"

namespace nvision
{
    /** Class to let FAST operate on a circle with diameter of 5 pixels.
      * The sequence length has be to be 9 to accept a pixel as corner. */
    class FASTMode5
    {
    public:
        using Matrixi = Eigen::Matrix<Index, 2, 12>;

        FASTMode5()
        {
            _circle <<  0,  1,  2, 2, 2, 1, 0, -1, -2, -2, -2, -1,
                       -2, -2, -1, 0, 1, 2, 2,  2,  1,  0, -1, -2;
        }

        constexpr Index sequence() const
        {
            return 9;
        }

        const Matrixi &circle() const
        {
            return _circle;
        }
    private:
        Matrixi _circle = {};
    };

    /** Class to let FAST operate on a circle with diameter of 7 pixels.
      * The sequence length has be to be 12 to accept a pixel as corner. */
    class FASTMode7
    {
    public:
        using Matrixi = Eigen::Matrix<Index, 2, 16>;

        FASTMode7()
        {
            _circle <<  0,  1,  2,  3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1,
                       -3, -3, -2, -1, 0, 1, 2, 3, 3,  3,  2,  1,  0, -1, -2, -3;
        }

        constexpr Index sequence() const
        {
            return 12;
        }

        const Matrixi &circle() const
        {
            return _circle;
        }
    private:
        Matrixi _circle = {};
    };

    /** Class to let FAST operate on a circle with diameter of 9 pixels.
      * The sequence length has be to be 15 to accept a pixel as corner. */
    class FASTMode9
    {
    public:
        using Matrixi = Eigen::Matrix<Index, 2, 20>;

        FASTMode9()
        {
            _circle <<  0,  1,  2,  3,  4, 4, 4, 3, 2, 1, 0, -1, -2, -3, -4, -4, -4, -3, -2, -1,
                       -4, -4, -3, -2, -1, 0, 1, 2, 3, 4, 4,  4,  3,  2,  1,  0, -1, -2, -3, -4;
        }

        constexpr Index sequence() const
        {
            return 15;
        }

        const Matrixi &circle() const
        {
            return _circle;
        }
    private:
        Matrixi _circle = {};
    };

    /** Computes the harris score for FAST corner detection. */
    template<typename _Scalar,
        typename _SmoothFilter=GaussFilter<_Scalar, 3>,
        typename _GradientFilter=SobelFilter<_Scalar>>
    class FASTHarrisScore
    {
    public:
        using Scalar = _Scalar;
        using SmoothFilter = _SmoothFilter;
        using GradientFilter = _GradientFilter;
        using Matrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> ;
        using Matrix2 = Eigen::Matrix<Scalar, 2, 2>;
        using Vector2i = Eigen::Matrix<Index, 2, 1>;

        static_assert(Eigen::NumTraits<Scalar>::IsInteger == 0, "Scalar must be floating point");

        FASTHarrisScore() = default;

        FASTHarrisScore(const Scalar traceFactor)
            : _traceFactor(traceFactor)
        { }

        void setTraceFactor(const Scalar factor)
        {
            _traceFactor = factor;
        }

        void setSmoothFilter(const SmoothFilter &filter)
        {
            _smooth = filter;
        }

        void setGradientFilter(const GradientFilter &filter)
        {
            _gradient = filter;
        }

        template<typename Derived>
        void operator()(const ImageBase<Derived> &img,
                        const std::vector<Vector2i> &corners,
                        const Scalar,
                        Matrix &score) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be of image type");
            using ColorSpace = ImageBase<Derived>::Scalar::ColorSpace;
            static_assert(ColorSpace::Dimension == 1, "FAST only supports single channel images");

            // calculate gradients
            Image<ColorSpace> gradX = _gradient(img, GradientMode::X());
            Image<ColorSpace> gradY = _gradient(img, GradientMode::Y());

            // accumulate values from local neighbourhood with smooth filter
            Image<ColorSpace> tmp = gradX * gradX;
            Image<ColorSpace> gradXX = _smooth(tmp);
            tmp = gradY * gradY;
            Image<ColorSpace> gradYY = _smooth(tmp);
            tmp = gradX * gradY;
            Image<ColorSpace> gradXY = _smooth(tmp);

            // compute the harris response
            score.resize(img.rows(), img.cols());
            score.setZero();

            for(size_t i = 0; i < corners.size(); ++i)
            {
                const auto r = corners[i](1);
                const auto c = corners[i](0);

                Matrix2 M;
                M << gradXX(r, c)[0], gradXY(r, c)[0],
                     gradXY(r, c)[0], gradYY(r, c)[0];

                Scalar trace = M(0, 0) + M(1, 1);
                Scalar det = M(0, 0) * M(1, 1) - M(0, 1) * M(1, 0);
                score(r, c) = det - _traceFactor * trace * trace;
            }
        }
    private:
        Scalar _traceFactor = static_cast<Scalar>(0.04);
        SmoothFilter _smooth = {};
        GradientFilter _gradient = {};
    };

    /** Computes the FAST score for FAST corner detection */
    template<typename _Scalar, typename _Mode>
    class FASTScore
    {
    public:
        using Scalar = _Scalar;
        using Mode = _Mode;
        using Matrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
        using Vector2i = Eigen::Matrix<Index, 2, 1>;

        static_assert(Eigen::NumTraits<Scalar>::IsInteger == 0, "Scalar must be floating point");

        FASTScore() = default;

        template<typename Derived>
        void operator()(const ImageBase<Derived> &img,
                        const std::vector<Vector2i> &corners,
                        const Scalar threshold,
                        Matrix &score) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be of image type");
            using ColorSpace = ImageBase<Derived>::Scalar::ColorSpace;
            static_assert(ColorSpace::Dimension == 1, "FAST only supports single channel images");

            score.resize(img.rows(), img.cols());
            score.setZero();
            for(size_t i = 0; i < corners.size(); ++i)
            {
                const auto r = corners[i](1);
                const auto c = corners[i](0);
                score(r, c) = computeScore(img, r, c, threshold);
            }
        }

    private:
        Mode _mode = {};

        template<typename Derived>
        Scalar computeScore(const ImageBase<Derived> &img,
                            const Index row,
                            const Index col,
                            const Scalar threshold) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be of image type");
            using ColorSpace = ImageBase<Derived>::Scalar::ColorSpace;
            const auto imageVal = static_cast<Scalar>(img(row, col)[0]);

            // determine lower intensity boundary
            const auto low = imageVal - threshold;
            // determine upper intensity boundary
            const auto high = imageVal + threshold;

            Scalar scoreLow = 0;
            Scalar scoreHigh = 0;
            for(Index i = 0; i < _mode.circle().cols(); ++i)
            {
                const auto r2 = row + _mode.circle()(1, i);
                const auto c2 = col + _mode.circle()(0, i);

                if(image::inside(img, r2, c2))
                {
                    const auto neighVal = static_cast<Scalar>(img(r2, c2)[0]);
                    if(neighVal < low)
                        scoreLow += std::abs(imageVal - neighVal);
                    else if(neighVal > high)
                        scoreHigh += std::abs(imageVal - neighVal);
                }
            }

            return std::max(scoreLow, scoreHigh);
        }
    };

    /** Base for FAST feature detection functors. */
    template<typename _Scalar, typename _Mode, typename _Score>
    class FASTFeatureBase
    {
    public:
        using Scalar = _Scalar;
        using Mode = _Mode;
        using Score = _Score;
        using Matrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
        using Vector2i = Eigen::Matrix<Index, 2, 1>;

        static_assert(Eigen::NumTraits<Scalar>::IsInteger == 0, "Scalar must be floating point");

        /** Construct a FAST detector base with custom parameters.
          * @param threshold FAST threshold which determines if a pixel is brighter or darker
          * @param minDist minimum distance between keypoints in non-maxima suppression
          * @param maxFeatures maximum number of features that should be detected
          * @param useSuppresion determine if non-maxima suppression is used */
        FASTFeatureBase(const Scalar threshold,
            const Index minDist,
            const Index maxFeatures,
            const bool useSuppresion)
            : _threshold(), _minDist(minDist),
            _maxFeatures(maxFeatures), _useSuppression(useSuppresion)
        {
            setThreshold(threshold);
        }

        virtual ~FASTFeatureBase()
        { }

        /** Set the threshold in [0;1] after which a pixel is considered brighter / darker
          * than its corresponding keypoint.
          * A pixel is considered brighter / darker than its corresponding keypoint if:
          *
          * pixval < kpval - threshold
          *
          * pixval > kpval + threshold
          *
          * The threshold is automatically converted to the correct range for different color spaces.
          *
          * @param threshold threshold for pixel comparison
          */
        void setThreshold(const Scalar threshold)
        {
            assert(threshold >= static_cast<Scalar>(0) && threshold <= static_cast<Scalar>(1));

            _threshold = nvision::clamp<Scalar>(threshold, 0, 1);
        }

        /** Set the minimum distance between two keypoints for non-maxima
          * suppression.
          * @param minDist minimum distance between keypoints */
        void setMinDistance(const Index minDist)
        {
            _minDist = minDist;
        }

        /** Set the maximum number of features, which should be detected.
          * If more features are found, the most suitable features are selected
          * according to a cornerness score.
          * @param maxFeatures maximum number of features */
        void setMaxFeatures(const Index maxFeatures)
        {
            _maxFeatures = maxFeatures;
        }

        /** En- or disable non-maxima suppression.
          * @param useSuppresion set true to enable non-maxima suppression */
        void setNonMaximaSuppression(const bool useSuppression)
        {
            _useSuppression = useSuppression;
        }

        void setMode(const Mode &mode)
        {
            _mode = mode;
        }

        void setScore(const Score &score)
        {
            _score = score;
        }

        Index minDistance() const
        {
            return _minDist;
        }

        bool useNonMaximaSuppression() const
        {
            return _useSuppression;
        }

        Index maxFeatures() const
        {
            return _maxFeatures;
        }

    private:
        Mode _mode = {};
        Score _score = {};
        Scalar _threshold = {};
        Index _minDist = {};
        Index _maxFeatures = {};
        bool _useSuppression = {};

    protected:

        enum class IntensityClass
        {
            None = 0,
            Lower,
            Higher
        };

        template<typename ColorSpace>
        Scalar realThreshold() const
        {
            const auto offset = static_cast<Scalar>(ColorSpace::minimum[0]);
            const auto factor = static_cast<Scalar>(ColorSpace::maximum[0]) - static_cast<Scalar>(ColorSpace::minimum[0]);
            return  offset + _threshold * factor;
        }

        template<typename Derived>
        void computeScore(const ImageBase<Derived> &img, const std::vector<Vector2i> &corners, Matrix &score) const
        {
            using ColorSpace = ImageBase<Derived>::Scalar::ColorSpace;
            _score(img, corners, realThreshold<ColorSpace>(), score);
        }

        template<typename Derived>
        IntensityClass determineClass(const ImageBase<Derived> &img,
                                      const Index row,
                                      const Index col,
                                      const Scalar low,
                                      const Scalar high) const
        {
            if(!image::inside(img, row, col))
                return IntensityClass::None;

            const auto value = static_cast<Scalar>(img(row, col)[0]);
            if(value < low)
                return IntensityClass::Lower;
            else if(value > high)
                return IntensityClass::Higher;
            else
                return IntensityClass::None;
        }

        template<typename Derived>
        bool fastCornerTest(const ImageBase<Derived> &img,
                            const Index row,
                            const Index col) const
        {
            using ColorSpace = ImageBase<Derived>::Scalar::ColorSpace;

            const auto imageVal = static_cast<Scalar>(img(row, col)[0]);
            const auto threshold = realThreshold<ColorSpace>();
            // determine lower intensity boundary
            const auto low = imageVal - threshold;
            // determine upper intensity boundary
            const auto high = imageVal + threshold;

            const auto quart = _mode.circle().cols() / 4;

            const auto north = determineClass(img, row + _mode.circle()(1, 0),       col + _mode.circle()(0, 0), low, high);
            const auto east  = determineClass(img, row + _mode.circle()(1, quart),   col + _mode.circle()(0, quart), low, high);
            const auto south = determineClass(img, row + _mode.circle()(1, 2*quart), col + _mode.circle()(0, 2*quart), low, high);
            const auto west  = determineClass(img, row + _mode.circle()(1, 3*quart), col + _mode.circle()(0, 3*quart), low, high);

            const auto testA = north == east && east == south;
            const auto testB = east == south && south == west;
            const auto testC = south == west && west == north;

            return testA || testB || testC;
        }

        template<typename Derived>
        bool fullCornerTest(const ImageBase<Derived> &img,
                            const Index row,
                            const Index col) const
        {
            using ColorSpace = ImageBase<Derived>::Scalar::ColorSpace;

            const auto imageVal = static_cast<Scalar>(img(row, col)[0]);
            const auto threshold = realThreshold<ColorSpace>();
            // determine lower intensity boundary
            const auto low = imageVal - threshold;
            // determine upper intensity boundary
            const auto high = imageVal + threshold;

            // count the length of sequence of pixels which have same intensity
            // class
            Index cnt = 0;
            // always keep track of the previous intesity class
            IntensityClass prev = IntensityClass::None;
            for(Index i = 0; i < _mode.circle().cols(); ++i)
            {
                // compute pixel coordinates based on given circle
                const auto r2 = row + _mode.circle()(1, i);
                const auto c2 = col + _mode.circle()(0, i);
                // determine the intensity class of this pixel
                const auto curr = determineClass(img, r2, c2, low, high);

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
                if(cnt >= _mode.sequence())
                    return true;
            }

            // if sequence length is zero there was no corner found
            if(cnt == 0)
                return false;

            for(Index i = 0; i <  _mode.sequence(); ++i)
            {
                // compute pixel coordinates based on given circle
                const auto r2 = row + _mode.circle()(1, i);
                const auto c2 = col + _mode.circle()(0, i);
                // determine the intensity class of this pixel
                const auto curr = determineClass(img, r2, c2, low, high);

                // if current pixel has no intensity class or it stops the
                // sequence, the return false, no corner detected
                if(curr == IntensityClass::None || curr != prev)
                    return false;
                else
                    ++cnt;
                prev = curr;

                if(cnt >= _mode.sequence())
                    return true;
            }

            return false;
        }

        template<typename Derived>
        void detectCorners(const ImageBase<Derived> &img,
                           std::vector<Vector2i> &corners) const
        {
            using ColorSpace = ImageBase<Derived>::Scalar::ColorSpace;
            static_assert(ColorSpace::Dimension == 1, "FAST only supports single channel images");

            for(Index c = 0; c < img.cols(); ++c)
            {
                for(Index r = 0; r < img.rows(); ++r)
                {
                    if(fastCornerTest(img, r, c) && fullCornerTest(img, r, c))
                        corners.push_back({c, r});
                }
            }
        }

        void nonMaximaSuppression(const Matrix &score,
                                  std::vector<Vector2i> &corners) const
        {
            std::vector<Vector2i> tmpCorners;
            tmpCorners.reserve(corners.size());
            for(size_t i = 0; i < corners.size(); ++i)
            {
                const auto c = corners[i](0);
                const auto r = corners[i](1);
                const auto val = score(r, c);

                bool best = true;
                for(Index x =  c - _minDist; x < c + _minDist; ++x)
                {
                    for(Index y =  r - _minDist; y < r + _minDist; ++y)
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
    };
}

#endif
