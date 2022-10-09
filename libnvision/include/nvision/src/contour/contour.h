/// @author Fabian Meyer
/// @date 28 Sep 2022
/// @file

#ifndef NVISION_CONTOUR_H_
#define NVISION_CONTOUR_H_

#include "nvision/src/core/types.h"
#include <Eigen/Dense>

namespace nvision
{
    using Contour = Eigen::Matrix<Index, 2, Eigen::Dynamic>;
}

#endif
