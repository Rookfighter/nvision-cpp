/* point_normalization.h
 *
 * Author: Fabian Meyer
 * Created On: 10 Sep 2019
 */

#ifndef CVE_POINT_NORMALIZATION_H_
#define CVE_POINT_NORMALIZATION_H_

#include "cve/core/assert.h"
#include "cve/core/matrix.h"

namespace cve
{
    template<typename Scalar>
    struct PointNormalization2D
    {
        typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
        typedef Eigen::Matrix<Scalar, 3, 3> Matrix3;

        template<typename Derived>
        Matrix3 operator()(const Eigen::MatrixBase<Derived> &points) const
        {
            CVE_ASSERT(points.rows() == 2);
            CVE_ASSERT(dest.rows() == 2);
            CVE_ASSERT(points.cols() == dest.cols());

            const Scalar sqrt2 = static_cast<Scalar>(1.414213562373095);

            Vector2 centroid = points.rowwise().mean();
            Scalar meanLen = (points.colwise() - centroid).colwise().norm().mean();
            Scalar scale = sqrt2 / meanLen;
            Matrix3 result;
            result <<
                scale, 0, -scale * centroid(0),
                0, scale, -scale * centroid(1),
                0, 0, 1;
            return result;
        }
    };
}

#endif
