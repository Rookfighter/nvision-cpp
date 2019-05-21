/* color_map.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef CVE_OPTFLOW_COLOR_MAP_H_
#define CVE_OPTFLOW_COLOR_MAP_H_

#include <Eigen/Geometry>
#include "cve/color_converter.h"
#include "cve/math.h"

namespace cve
{
    namespace optflow
    {
        template<typename Scalar>
        class ColorMap
        {
        public:
            ColorMap()
            {}

            template<typename ScalarA>
            void apply(const Eigen::Tensor<Scalar, 3> &flowImg,
                Eigen::Tensor<ScalarA, 3> &img) const
            {
                assert(flowImg.dimension(2) == 2);

                img.resize(flowImg.dimension(0), flowImg.dimension(1), 3);
                for(Index c = 0; c < flowImg.dimension(1); ++c)
                {
                    for(Index r = 0; r < flowImg.dimension(0); ++r)
                    {
                        Eigen::Matrix<Scalar, 2, 1> vec;
                        vec << flowImg(r, c, 0), flowImg(r, c, 1);

                        Scalar phi = std::atan2(vec(1), vec(0));
                        if(phi < 0)
                            phi += 2 * pi<Scalar>();

                        Scalar intense = std::min<Scalar>(1, vec.norm());
                        if(intense > 1)
                            intense = 1;
                        Eigen::Array<Scalar, 3, 1> hsv;
                        hsv(0) = phi * 180 / pi<Scalar>();
                        hsv(1) = intense;
                        hsv(2) = 0.9;

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
