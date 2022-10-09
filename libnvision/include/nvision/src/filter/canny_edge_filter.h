/// @author Fabian Meyer
/// @date 13 may 2019
/// @file

#ifndef NVISION_CANNY_EDGE_FILTER_H_
#define NVISION_CANNY_EDGE_FILTER_H_

#include "nvision/src/core/image.h"

namespace nvision
{
    /// @brief Edge detection filter using the Canny algorithm.
    /// @tparam _Scalar Scalar type that is used for internal computations
    /// @see https://en.wikipedia.org/wiki/Canny_edge_detector
    template<typename _Scalar>
    class CannyEdgeFilter
    {
    public:
        using Scalar = _Scalar;

        CannyEdgeFilter() = default;

        CannyEdgeFilter(const Scalar low, const Scalar high)
            : _lowThreshold(low), _highThreshold(high)
        { }

        void setThreshold(const Scalar low, const Scalar high)
        {
            _lowThreshold = low;
            _highThreshold = high;
        }

        /// @brief Computes edges from the given gradients.
        /// @tparam Derived
        /// @param gradientX image gradient in x-direction
        /// @param gradientY image gradient in y-direction
        /// @return binary edge image where all non-zero entries are considered edges
        template<typename Derived>
        auto operator()(const ImageBase<Derived> &gradientX, const ImageBase<Derived> &gradientY) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");
            using PixelType = typename ImageBase<Derived>::Scalar;
            using ColorSpace = typename PixelType::ColorSpace;
            using ValueType = typename ColorSpace::ValueType;

            assert(gradientX.cols() == gradientY.cols());
            assert(gradientX.rows() == gradientY.rows());

            auto edgeImage = Image<ColorSpace>{gradientX.rows(), gradientX.cols()};

            for(auto c = Index{0}; c < gradientX.cols(); ++c)
            {
                for(auto r = Index{0}; r < gradientX.rows(); ++r)
                {
                    for(auto d = Index{0}; d < ColorSpace::Dimension;++d)
                    {
                        // apply non-maximum suppression
                        const auto isEdge = computeNonMaximaSuppression(gradientX, gradientY, r, c, d);
                        edgeImage(r, c)[d] = isEdge ? ValueType{1} : ValueType{0};
                    }
                }
            }

            for(auto c = Index{0}; c < gradientX.cols(); ++c)
            {
                for(auto r = Index{0}; r < gradientX.rows(); ++r)
                {
                    for(auto d = Index{0}; d < ColorSpace::Dimension;++d)
                    {
                        if(edgeImage(r, c)[d] == ValueType{0})
                        {
                            continue;
                        }

                        // apply non-maximum suppression and
                        // perform hysteresis and follow edges
                        const auto isEdge = computeHyteresis(gradientX, gradientY, edgeImage, r, c, d);
                        edgeImage(r, c)[d] = isEdge ? ValueType{1} : ValueType{0};
                    }
                }
            }

            return edgeImage;
        }

    private:
        Scalar _lowThreshold = static_cast<Scalar>(0.078f);
        Scalar _highThreshold = static_cast<Scalar>(0.3992f);

        template<typename Derived>
        Scalar getMagnitudeSq(const ImageBase<Derived> &gradientX,
                              const ImageBase<Derived> &gradientY,
                              const Index row,
                              const Index col,
                              const Index depth) const
        {
            if(!image::inside(gradientX, row, col))
            {
                return Scalar{0};
            }
            else
            {
                const auto dx = static_cast<Scalar>(gradientX(row, col)[depth]);
                const auto dy = static_cast<Scalar>(gradientY(row, col)[depth]);
                return dx * dx + dy * dy;
            }
        }

        template<typename Derived>
        bool computeNonMaximaSuppression(const ImageBase<Derived> &gradientX,
                                         const ImageBase<Derived> &gradientY,
                                         const Index row,
                                         const Index col,
                                         const Index depth) const
        {
            static constexpr auto neighbor_map = std::array<std::pair<Index, Index>, 4>{
                std::make_pair(1, 0),
                std::make_pair(1, 1),
                std::make_pair(0, 1),
                std::make_pair(-1, 1)
            };
            static constexpr auto pi_quarter = pi<Scalar>() / Scalar{4};
            static constexpr auto pi_eighth = pi<Scalar>() / Scalar{8};

            const auto dx = static_cast<Scalar>(gradientX(row, col)[depth]);
            const auto dy = static_cast<Scalar>(gradientY(row, col)[depth]);
            const auto angle = std::atan2(dy, dx) + pi<Scalar>() + pi_eighth;
            const auto direction = static_cast<Index>(angle / pi_quarter) % neighbor_map.size();

            const auto cA = neighbor_map[direction].first;
            const auto rA = neighbor_map[direction].second;
            const auto cB = -neighbor_map[direction].first;
            const auto rB = -neighbor_map[direction].second;

            const auto magnitudeSq = dx * dx + dy * dy;
            const auto magnitudeSqA = getMagnitudeSq(gradientX, gradientY, rA, cA, depth);
            const auto magnitudeSqB = getMagnitudeSq(gradientX, gradientY, rB, cB, depth);

            // if both of the neighbors is stronger suppress this pixel
            return magnitudeSq >= magnitudeSqA || magnitudeSq >= magnitudeSqB;
        }

        template<typename Derived>
        bool computeHyteresis(const ImageBase<Derived> &gradientX,
                              const ImageBase<Derived> &gradientY,
                              const ImageBase<Derived> &edgeImage,
                              const Index row,
                              const Index col,
                              const Index depth) const
        {
            using PixelType = typename ImageBase<Derived>::Scalar;
            using ColorSpace = typename PixelType::ColorSpace;
            using ValueType = typename ColorSpace::ValueType;

            const auto lowThresholdSq = _lowThreshold * _lowThreshold;
            const auto highThresholdSq = _highThreshold * _highThreshold;

            const auto dx = static_cast<Scalar>(gradientX(row, col)[depth]);
            const auto dy = static_cast<Scalar>(gradientY(row, col)[depth]);
            const auto magnitudeSq = dx * dx + dy * dy;

            // if magnitude is below low threshold, suppress this pixel
            if(magnitudeSq < lowThresholdSq)
            {
                return false;
            }
            // if any neighbor is strong then do not suppress this pixel
            else if(magnitudeSq < highThresholdSq)
            {
                bool suppress = true;
                for(auto dcol = Index{0}; dcol < Index{3}; ++dcol)
                {
                    const auto col2 = col + dcol - Index{1};
                    for(Index drow = Index{0}; drow < Index{3}; ++drow)
                    {
                        const auto row2 = row + drow - Index{1};
                        if(!image::inside(edgeImage, row2, col2) || edgeImage(row2, col2)[depth] == ValueType{0})
                        {
                            continue;
                        }

                        const auto neighborMagnitudeSq = getMagnitudeSq(gradientX, gradientY, row2, col2, depth);
                        if(neighborMagnitudeSq >= highThresholdSq)
                        {
                            suppress = false;
                            break;
                        }
                    }

                    if(!suppress)
                    {
                        break;
                    }
                }

                return !suppress;
            }
            // accept all strong pixels
            else
            {
                return true;
            }
        }
    };
}

#endif
