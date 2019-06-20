/* fast_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jun 2019
 */

#ifndef CVE_FAST_DETECTOR_H_
#define CVE_FAST_DETECTOR_H_

#include "cve/math.h"
#include "cve/image.h"

namespace cve
{
    /** Class for FAST corner detection. */
    template<typename Scalar>
    class FASTDetector
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;
        typedef Eigen::Matrix<Index, Eigen::Dynamic, Eigen::Dynamic> Matrixi;
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
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
    public:
        FASTDetector()
            : FASTDetector(50)
        {

        }

        FASTDetector(const Scalar threshold)
            : threshold_(threshold), circle_(2, 16), neighs_(12)
        {
            circle_ <<  0,  1,  2,  3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1,
                       -3, -3, -2, -1, 0, 1, 2, 3, 3,  3,  2,  1,  0, -1, -2, -3;
        }

        void setThreshold(const Scalar threshold)
        {
            threshold_ = threshold;
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
        void detect(const Eigen::Tensor<ScalarA, 3> &img,
            Matrix &keypoints) const
        {
            std::vector<Vector2> points;
            points.reserve(5000);
            for(Index c = 0; c < img.dimension(1); ++c)
            {
                for(Index r = 0; r < img.dimension(0); ++r)
                {
                    if(isCorner(r, c, img))
                        points.push_back({c, r});
                }
            }

            keypoints.resize(2, points.size());
            for(size_t i = 0; i < points.size(); ++i)
                keypoints.col(i) = points[i];
        }
    };
}

#endif
