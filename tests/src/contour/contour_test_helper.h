/// @author Fabian Meyer
/// @date 28 Sep 2022
/// @file

#ifndef NVISION_CONTOUR_TEST_HELPER_H_
#define NVISION_CONTOUR_TEST_HELPER_H_

#include <catch2/catch.hpp>
#include "nvision/src/core/image.h"
#include "nvision/src/contour/contour.h"

using namespace nvision;


template<typename Derived>
inline void makeContourImage(ImageBase<Derived> &image, const std::vector<Contour> &contours)
{
    using PixelType = typename ImageBase<Derived>::Scalar;
    image.setZero();

    for(const auto &contour: contours)
    {
        for(auto i = Index{0}; i < contour.cols(); ++i)
        {
            image(contour(1, i), contour(0, i)) = PixelType{1};
        }
    }
}

inline Contour makeLargeCircleContour(const Eigen::Matrix<Index, 2, 1> &translation)
{
    auto contour = Contour(2, 28);
    contour <<  3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 10, 9, 8,  7,  6,  5,  4,  3, 2, 1, 0, 0, 0, 0, 0, 1, 2,
                0, 0, 0, 0, 0, 1, 2,  3,  4,  5,  6,  7, 8, 9, 10, 10, 10, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1;
    contour.colwise() += translation;
    return contour;
}

inline Contour makeSmallCircleContour(const Eigen::Matrix<Index, 2, 1> &translation)
{
    auto contour = Contour(2, 16);
    contour << 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1, 0, 0, 0, 1,
               0, 0, 0, 1, 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1;
    contour.colwise() += translation;
    return contour;
}

inline Contour makeSquareContour()
{
    auto contour = Contour(2, 16);
    contour << 1, 2, 3, 4, 5, 1, 1, 1, 1, 2, 3, 4, 5, 5, 5, 5,
               1, 1, 1, 1, 1, 2, 3, 4, 5, 5, 5, 5, 5, 2, 3, 4;
    return contour;
}

inline void checkContours(const std::vector<Contour> &expected, const std::vector<Contour> &actual)
{
    REQUIRE(expected.size() == actual.size());

    size_t contourCount = 0;
    for(auto i = size_t{0}; i < expected.size(); ++i)
    {
        for(auto j = size_t{0}; j < actual.size(); ++j)
        {
            if(actual[j].rows() != expected[i].rows() || actual[j].cols() != expected[i].cols())
            {
                continue;
            }

            Index pointCount = 0;
            for(auto c1 = Index{0}; c1 < expected[i].cols(); ++c1)
            {
                for(auto c2 = Index{0}; c2 < actual[j].cols(); ++c2)
                {
                    if(expected[i].col(c1) == actual[i].col(c2))
                    {
                        ++pointCount;
                        break;
                    }
                }
            }

            if(pointCount == expected[i].cols())
            {
                ++contourCount;
                break;
            }
        }
    }
    REQUIRE(contourCount == expected.size());
}

#endif
