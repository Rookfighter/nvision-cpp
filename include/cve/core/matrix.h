/* matrix.h
 *
 * Author: Fabian Meyer
 * Created On: 18 Jul 2019
 */

#ifndef CVE_MATRIX_H_
#define CVE_MATRIX_H_

#include <Eigen/Geometry>

namespace cve
{
    typedef long int Index;

    typedef Eigen::Matrix<Index, Eigen::Dynamic, 1> Vectori;
    typedef Eigen::Matrix<Index, Eigen::Dynamic, Eigen::Dynamic> Matrixi;
    typedef Eigen::Matrix<uint8_t, Eigen::Dynamic, 1> Vectoru8;
    typedef Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu8;
    typedef Eigen::Matrix<uint16_t, Eigen::Dynamic, 1> Vectoru16;
    typedef Eigen::Matrix<uint16_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu16;
    typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, 1> Vectoru32;
    typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu32;
}

#endif
