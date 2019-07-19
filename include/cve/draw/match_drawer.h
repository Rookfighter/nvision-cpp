/* match_drawer.h
 *
 * Author: Fabian Meyer
 * Created On: 19 Jul 2019
 */

#ifndef CVE_MATCH_DRAWER_H_
#define CVE_MATCH_DRAWER_H_

#include "cve/draw/shape_drawer.h"
#include "cve/draw/colors.h"

namespace cve
{
    template<typename Scalar>
    class MatchDrawer
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        MatchDrawer()
        {

        }

        template<typename ScalarA, typename ScalarB,
            typename DerivedA, typename DerivedB, typename DerivedC>
        void draw(const Eigen::Tensor<ScalarA, 3> &imgA,
            const Eigen::Tensor<ScalarA, 3> &imgB,
            const Eigen::MatrixBase<DerivedA> &keypointsA,
            const Eigen::MatrixBase<DerivedB> &keypointsB,
            const Eigen::MatrixBase<DerivedC> &indices,
            Eigen::Tensor<ScalarB, 3> &img) const
        {
            assert(keypointsA.rows() == 2);
            assert(keypointsB.rows() == 2);
            assert(indices.rows() >= 1);
            assert(keypointsA.cols() == indices.cols());
            assert(imgA.dimension(2) >= 3);
            assert(imgB.dimension(2) >= 3);

            ColorSeries<ScalarB> colorSeries;
            ShapeDrawer<Scalar> shapeDrawer;

            img.resize(std::max(imgA.dimension(0), imgB.dimension(0)),
                imgA.dimension(1) + imgB.dimension(1), 3);
            img.setZero();
            // copy image A onto result image
            for(Index d = 0; d < imgA.dimension(2); ++d)
                for(Index c = 0; c < imgA.dimension(1); ++c)
                    for(Index r = 0; r < imgA.dimension(0); ++r)
                        img(r, c, d) = imgA(r, c, d);
            // copy image B onto result image
            for(Index d = 0; d < imgB.dimension(2); ++d)
                for(Index c = 0; c < imgB.dimension(1); ++c)
                    for(Index r = 0; r < imgB.dimension(0); ++r)
                        img(r, c + imgA.dimension(1), d) = imgB(r, c, d);

            Eigen::Matrix<Scalar, 2, 2> points;
            Eigen::Array<ScalarB, 3, 1> color;
            for(Index i = 0; i < indices.cols(); ++i)
            {
                if(indices(0, i) < 0 || indices(0, i) >= keypointsB.cols())
                    continue;

                color = colorSeries.next();
                points.col(0) = keypointsA.col(i);
                points.col(1) = keypointsB.col(indices(0, i));
                points(0, 1) += imgA.dimension(1);
                shapeDrawer.drawCircle(points, 7, color, img);
            }
        }
    };
}

#endif
