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
            typedef Image<Scalar, 2> FlowImage;

            ColorMap()
            {}

            template<typename Image>
            void apply(const FlowImage &flowImg, Image &img) const
            {
                static_assert(Image::Depth >= 3, "Image needs at least depth of 3");

                Eigen::Array<Scalar, Eigen::Dynamic, Eigen::Dynamic> intensity;
                intensity.resize(flowImg.rows(), flowImg.cols());
                for(Index c = 0; c < flowImg.cols(); ++c)
                    for(Index r = 0; r < flowImg.rows(); ++r)
                        intensity(r, c) = flowImg(r, c).matrix().norm();
                intensity -= intensity.minCoeff();
                intensity /= intensity.maxCoeff();

                img.resize(flowImg.rows(), flowImg.cols());
                for(Index c = 0; c < flowImg.cols(); ++c)
                {
                    for(Index r = 0; r < flowImg.rows(); ++r)
                    {
                        Eigen::Matrix<Scalar, 2, 1> vec = flowImg(r, c).matrix();

                        Scalar phi = std::atan2(vec(1), vec(0));
                        if(phi < 0)
                            phi += 2 * pi<Scalar>();

                        Scalar intense = intensity(r, c);
                        Eigen::Array<Scalar, 3, 1> hsv;
                        hsv(0) = phi * 180 / pi<Scalar>();
                        hsv(1) = intense;
                        hsv(2) = 0.9;

                        ColorConverter conv;
                        img(r, c) = conv.hsv2rgb<Scalar, typename Image::Scalar>(hsv);
                    }
                }
            }
        };
    }
}

#endif
