/* flow_field.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef NVISION_FLOW_FIELD_H_
#define NVISION_FLOW_FIELD_H_

#include <Eigen/Core>

namespace nvision
{
    template<typename Scalar>
    using FlowVector = Eigen::Matrix<Scalar, 2, 1>;

    template<typename Scalar>
    using FlowField = Eigen::Array<FlowVector<Scalar>, Eigen::Dynamic, Eigen::Dynamic>;
}

#endif
