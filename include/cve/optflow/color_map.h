/* color_map.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef CVE_OPTFLOW_COLOR_MAP_H_
#define CVE_OPTFLOW_COLOR_MAP_H_

#include <Eigen/Geometry>
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

                img.resize(flowImg.rows(), flowImg.cols());
                for(Index c = 0; c < flowImg.cols(); ++c)
                {
                    for(Index r = 0; r < flowImg.rows(); ++r)
                    {
                        Eigen::Matrix<Scalar, 2, 1> vec = flowImg(r, c).matrix();

                        Scalar phi = std::atan2(vec(1), vec(0)) + pi<Scalar>();

                        Scalar intense = vec.norm();
                        if(intense > 1)
                            intense = 0;

                        Scalar alpha;
                        Scalar beta;
                        Scalar piquart = pi<Scalar>() / 4;
                        if(phi < piquart)
                        {
                            beta  = phi / piquart;
                            alpha = 1.0 - beta;
                            img(r, c, 0) = intense * (alpha * 255 + beta * 255);
                            img(r, c, 1) = 0;
                            img(r, c, 2) = intense * beta * 255;
                        }
                        else if(phi < 2 * piquart)
                        {
                            beta  = (phi - piquart) / piquart;
                            alpha = 1.0 - beta;
                            img(r, c, 0) = intense * (alpha * 255 + beta *  64);
                            img(r, c, 1) = intense * (beta * 64);
                            img(r, c, 2) = intense * (alpha * 255 + beta * 255);
                        }
                        else if(phi < 3 * piquart)
                        {
                            beta  = (phi - 2 * piquart) / piquart;
                            alpha = 1.0 - beta;
                            img(r, c, 0) = intense * (alpha * 64);
                            img(r, c, 1) = intense * (alpha * 64 + beta * 255);
                            img(r, c, 2) = intense * (alpha * 255 + beta * 255);
                        }
                        else if(phi < 4 * piquart)
                        {
                            beta  = (phi - 3 * piquart) / piquart;
                            alpha = 1.0 - beta;
                            img(r, c, 0) = 0;
                            img(r, c, 1) = intense * (alpha * 255 + beta * 255);
                            img(r, c, 2) = intense * (alpha * 255);
                        }
                        else if(phi < 6 * piquart)
                        {
                            beta  = (phi - 4 * piquart) / (2 * piquart);
                            alpha = 1.0 - beta;
                            img(r, c, 0) = intense * (beta * 255);
                            img(r, c, 1) = intense * (alpha * 255 + beta * 255);
                            img(r, c, 2) = 0;
                        }
                        else if (phi <= 8 * piquart)
                        {
                            beta  = (phi - 6 * piquart) / (2 * piquart);
                            alpha = 1.0 - beta;
                            img(r, c, 0) = intense * (alpha * 255 + beta * 255);
                            img(r, c, 1) = intense * (alpha * 255);
                            img(r, c, 2) = 0;
                        }
                    }
                }
            }
        };
    }
}

#endif
