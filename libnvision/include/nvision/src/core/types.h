/* types.h
 *
 * Author: Fabian Meyer
 * Created On: 23 Dec 2021
 */


#ifndef NVISION_TYPES_H_
#define NVISION_TYPES_H_

#include <Eigen/Core>
#include <cstdint>

namespace nvision
{
    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;

    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;

    using float32 = float;
    using float64 = double;

    using Index = Eigen::Index;
}

#endif
