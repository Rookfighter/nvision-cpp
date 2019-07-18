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
    typedef Eigen::Matrix<int8_t, Eigen::Dynamic, 1> Vectori8;
    typedef Eigen::Matrix<int16_t, Eigen::Dynamic, 1> Vectori16;
    typedef Eigen::Matrix<int32_t, Eigen::Dynamic, 1> Vectori32;
    typedef Eigen::Matrix<int64_t, Eigen::Dynamic, 1> Vectori64;

    typedef Eigen::Matrix<uint8_t, Eigen::Dynamic, 1> Vectoru8;
    typedef Eigen::Matrix<uint16_t, Eigen::Dynamic, 1> Vectoru16;
    typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, 1> Vectoru32;
    typedef Eigen::Matrix<uint64_t, Eigen::Dynamic, 1> Vectoru64;

    typedef Eigen::Matrix<Index, Eigen::Dynamic, Eigen::Dynamic> Matrixi;
    typedef Eigen::Matrix<int8_t, Eigen::Dynamic, Eigen::Dynamic> Matrixi8;
    typedef Eigen::Matrix<int16_t, Eigen::Dynamic, Eigen::Dynamic> Matrixi16;
    typedef Eigen::Matrix<int32_t, Eigen::Dynamic, Eigen::Dynamic> Matrixi32;
    typedef Eigen::Matrix<int64_t, Eigen::Dynamic, Eigen::Dynamic> Matrixi64;


    typedef Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu8;
    typedef Eigen::Matrix<uint16_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu16;
    typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu32;
    typedef Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu64;
}

#endif
