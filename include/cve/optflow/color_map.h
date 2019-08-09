/* color_map.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef CVE_OPTFLOW_COLOR_MAP_H_
#define CVE_OPTFLOW_COLOR_MAP_H_

#include "cve/draw/color_converter.h"
#include "cve/core/math.h"

namespace cve
{
    namespace optflow
    {
        template<typename Scalar>
        class ColorMap
        {
        public:
            typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
            typedef Eigen::Array<Scalar, 3, 1> Array3;

            ColorMap()
            {}

            template<typename ScalarA>
            void apply(const Eigen::Tensor<Scalar, 3> &flowImg,
                Eigen::Tensor<ScalarA, 3> &img) const
            {
                assert(flowImg.dimension(2) == 2);

                Scalar maxIntense = 0;
                Eigen::Tensor<Scalar, 3> imgHsv(flowImg.dimension(0), flowImg.dimension(1), 3);
                for(Index c = 0; c < flowImg.dimension(1); ++c)
                {
                    for(Index r = 0; r < flowImg.dimension(0); ++r)
                    {
                        Vector2 vec;
                        vec << flowImg(r, c, 0), flowImg(r, c, 1);

                        Scalar phi = angle::normalize(std::atan2(vec(1), vec(0)));

                        Scalar intense = vec.norm();

                        imgHsv(r, c, 0) = phi * 180 / pi<Scalar>();
                        imgHsv(r, c, 1) = intense;
                        imgHsv(r, c, 2) = 0.9;
                        maxIntense = std::max<Scalar>(intense, maxIntense);
                    }
                }

                img.resize(flowImg.dimension(0), flowImg.dimension(1), 3);
                for(Index c = 0; c < flowImg.dimension(1); ++c)
                {
                    for(Index r = 0; r < flowImg.dimension(0); ++r)
                    {
                        Array3 hsv;
                        hsv(0) = imgHsv(r, c, 0);
                        hsv(1) = imgHsv(r, c, 1) / maxIntense;
                        hsv(2) = imgHsv(r, c, 2);

                        ColorConverter conv;
                        Eigen::Array<ScalarA, 3, 1> rgb = conv.hsv2rgb<Scalar, ScalarA>(hsv);
                        img(r, c, 0) = rgb(0);
                        img(r, c, 1) = rgb(1);
                        img(r, c, 2) = rgb(2);
                    }
                }
            }
        };
    }
}

#endif
