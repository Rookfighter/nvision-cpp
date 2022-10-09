/// @author Fabian Meyer
/// @date 28 Sep 2022
/// @file


#ifndef NVISION_PAVLIDIS_TRACER_H_
#define NVISION_PAVLIDIS_TRACER_H_

#include "nvision/src/core/image.h"
#include "nvision/src/contour/contour.h"

namespace nvision
{
    /// @brief Finds contours in binary images using the pavlidis algorithm.
    /// @see https://imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/theo.html
    class PavlidisTracer
    {
    public:
        template<typename Derived>
        void trace(const ImageBase<Derived> &image, std::vector<Contour> &contours)
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
            static_assert(ImageDepth<ImageBase<Derived>>::value == Index{1}, "can only handle single channle pixel types");

            using PixelType = typename ImageBase<Derived>::Scalar;
            using Coordinate = Eigen::Matrix<Index, 2, 1>;

            // initialize moore neighborhood
            static constexpr auto neighborCount = Index{8};
            static const auto neighbors = std::array<Coordinate, neighborCount>{
                Coordinate{1, -1},
                Coordinate{1, 0},
                Coordinate{1, 1},
                Coordinate{0, 1},
                Coordinate{-1, 1},
                Coordinate{-1, 0},
                Coordinate{-1, -1},
                Coordinate{0, -1}
            };

            // mapping of indices for backtracking
            static constexpr auto directionMap = std::array<Index, 4>{0, 2, 4, 6};

            // keeps track of which pixels have already been visited
            auto closedSet = Eigen::Array<uint8, Eigen::Dynamic, Eigen::Dynamic>{image.rows(), image.cols()};
            closedSet.setZero();

            contours.clear();
            auto contour = std::vector<Coordinate>{};
            for(auto c = Index{0}; c < image.cols(); ++c)
            {
                for(auto r = Index{0}; r < image.rows(); ++r)
                {
                    // skip pixels which are not edges or have been visited already
                    if(image(r, c) == PixelType{0} || closedSet(r, c) != uint8{0})
                    {
                        continue;
                    }

                    contour.clear();

                    auto direction = Index{0};
                    const auto startPoint = Coordinate{c, r};

                    auto hasNext = true;
                    auto nextPoint = startPoint;
                    while(hasNext)
                    {
                        const auto currentPoint = nextPoint;
                        closedSet(currentPoint.y(), currentPoint.x()) = uint8{1};
                        contour.push_back(currentPoint);

                        hasNext = false;

                        for(auto i = Index{0}; i < directionMap.size(); ++i)
                        {
                            auto localDirection = (direction + i) % directionMap.size();
                            auto hasStart = false;
                            for(auto j = Index{0}; j < 3; ++j)
                            {
                                const auto idx = (directionMap[localDirection] + j) % neighborCount;
                                auto candidatePoint = Coordinate{currentPoint + neighbors[idx]};

                                if(candidatePoint == startPoint)
                                {
                                    hasStart = true;
                                    break;
                                }

                                if(nvision::image::inside(image, candidatePoint.y(), candidatePoint.x()) &&
                                   image(candidatePoint.y(), candidatePoint.x()) != PixelType{0} &&
                                   closedSet(candidatePoint.y(), candidatePoint.x()) == uint8{0})
                                {
                                    hasNext= true;
                                    nextPoint = candidatePoint;
                                    direction =  (localDirection - std::max(Index{0}, Index{1} - j)) % directionMap.size();
                                    break;
                                }
                            }

                            if(hasNext || hasStart)
                            {
                                break;
                            }
                        }
                    }

                    contours.push_back({});
                    contours.back().resize(2, contour.size());
                    for(auto i = Index{0}; i < contours.back().cols(); ++i)
                    {
                        contours.back()(0, i) = contour[i].x();
                        contours.back()(1, i) = contour[i].y();
                    }
                }
            }
        }
    };
}

#endif
