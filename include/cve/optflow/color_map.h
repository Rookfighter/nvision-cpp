/* color_map.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef CVE_OPTFLOW_COLOR_MAP_H_
#define CVE_OPTFLOW_COLOR_MAP_H_

#include "cve/draw/color_converter.h"
#include "cve/core/image.h"
#include "cve/core/math.h"

namespace cve
{
    namespace optflow
    {
        template<typename Scalar>
        class ColorMap
        {
        private:
            template<typename ScalarA>
            void bright(const Eigen::Tensor<Scalar, 3> &flowMag,
                const Eigen::Tensor<Scalar, 3> &flowDir,
                Eigen::Tensor<ScalarA, 3> &img) const
            {
                img.resize(flowMag.dimension(0), flowMag.dimension(1), 3);
                for(Index c = 0; c < img.dimension(1); ++c)
                {
                    for(Index r = 0; r < img.dimension(0); ++r)
                    {
                        Array3 hsv;
                        Scalar ang = angle::normalize(flowDir(r, c, 0));
                        hsv(0) = angle::rad2deg(ang);
                        hsv(1) = flowMag(r, c, 0);
                        hsv(2) = 0.9;

                        ColorConverter conv;
                        Eigen::Array<ScalarA, 3, 1> rgb = conv.hsv2rgb<Scalar, ScalarA>(hsv);
                        img(r, c, 0) = rgb(0);
                        img(r, c, 1) = rgb(1);
                        img(r, c, 2) = rgb(2);
                    }
                }
            }

            template<typename ScalarA>
            void dark(const Eigen::Tensor<Scalar, 3> &flowMag,
                const Eigen::Tensor<Scalar, 3> &flowDir,
                Eigen::Tensor<ScalarA, 3> &img) const
            {
                img.resize(flowMag.dimension(0), flowMag.dimension(1), 3);
                for(Index c = 0; c < img.dimension(1); ++c)
                {
                    for(Index r = 0; r < img.dimension(0); ++r)
                    {
                        Array3 hsv;

                        Scalar ang = angle::normalize(flowDir(r, c, 0));
                        Scalar mag = flowMag(r, c, 0);

                        ScalarA R = 0;
                        ScalarA G = 0;
                        ScalarA B = 0;
                        ang *= 0.5;
                        if(ang >= 0 && ang <= 0.125 * pi<Scalar>())
                        {
                            Scalar beta = ang / (0.125 * pi<Scalar>());
                            Scalar alpha = 1 - beta;
                            R = (ScalarA)(mag * (alpha * 255 + beta * 255));
                            G = (ScalarA)(mag * (alpha *   0 + beta *   0));
                            B = (ScalarA)(mag * (alpha *   0 + beta * 255));
                        }
                        if(ang >= 0.125 * pi<Scalar>() && ang < 0.25 * pi<Scalar>())
                        {
                            Scalar beta  = (ang - 0.125 * pi<Scalar>()) / (0.125 * pi<Scalar>());
                            Scalar alpha = 1 - beta;
                            R = (ScalarA)(mag * (alpha * 255 + beta *  64));
                            G = (ScalarA)(mag * (alpha *   0 + beta *  64));
                            B = (ScalarA)(mag * (alpha * 255 + beta * 255));
                        }
                        /// Interpolation between blue (0.25 * pi<Scalar>()) and green (0.5 * pi<Scalar>())
                        if(ang >= 0.25 * pi<Scalar>() && ang < 0.375 * pi<Scalar>())
                        {
                            Scalar beta  = (ang - 0.25 * pi<Scalar>()) / (0.125 * pi<Scalar>());
                            Scalar alpha = 1 - beta;
                            R = (ScalarA)(mag * (alpha *  64 + beta *   0));
                            G = (ScalarA)(mag * (alpha *  64 + beta * 255));
                            B = (ScalarA)(mag * (alpha * 255 + beta * 255));
                        }
                        if(ang >= 0.375 * pi<Scalar>() && ang < 0.5 * pi<Scalar>())
                        {
                            Scalar beta  = (ang - 0.375 * pi<Scalar>()) / (0.125 * pi<Scalar>());
                            Scalar alpha = 1 - beta;
                            R = (ScalarA)(mag * (alpha *   0 + beta *   0));
                            G = (ScalarA)(mag * (alpha * 255 + beta * 255));
                            B = (ScalarA)(mag * (alpha * 255 + beta *   0));
                        }
                        /// Interpolation between green (0.5 * pi<Scalar>()) and yellow (0.75 * pi<Scalar>())
                        if(ang >= 0.5 * pi<Scalar>() && ang < 0.75 * pi<Scalar>())
                        {
                            Scalar beta  = (ang - 0.5 * pi<Scalar>()) / (0.25 * pi<Scalar>());
                            Scalar alpha = 1 - beta;
                            R = (ScalarA)(mag * (alpha *   0 + beta * 255));
                            G = (ScalarA)(mag * (alpha * 255 + beta * 255));
                            B = (ScalarA)(mag * (alpha *   0 + beta *   0));
                        }
                        /// Interpolation between yellow (0.75 * pi<Scalar>()) and red (Pi)
                        if(ang >= 0.75 * pi<Scalar>() && ang <= pi<Scalar>())
                        {
                            Scalar beta  = (ang - 0.75 * pi<Scalar>()) / (0.25 * pi<Scalar>());
                            Scalar alpha = 1 - beta;
                            R = (ScalarA)(mag * (alpha * 255 + beta * 255));
                            G = (ScalarA)(mag * (alpha * 255 + beta *   0));
                            B = (ScalarA)(mag * (alpha *   0 + beta *   0));
                        }

                        img(r, c, 0) = R;
                        img(r, c, 1) = G;
                        img(r, c, 2) = B;
                    }
                }
            }
        public:
            typedef Eigen::Matrix<Scalar, 2, 1> Vector2;
            typedef Eigen::Array<Scalar, 3, 1> Array3;

            ColorMap()
            {}

            template<typename ScalarA>
            void operator()(const Eigen::Tensor<Scalar, 3> &flowImg,
                Eigen::Tensor<ScalarA, 3> &img) const
            {
                assert(flowImg.dimension(2) == 2);

                Eigen::Tensor<Scalar, 3> flowMag;
                Eigen::Tensor<Scalar, 3> flowDir;
                image::magnitude(flowImg, flowMag);
                image::direction(flowImg, flowDir);
                image::normalize<Scalar>(flowMag, 0, 1);

                bright(flowMag, flowDir, img);
            }
        };

        template<typename Scalar>
        class ColorWheel
        {
        public:

            template<typename ScalarA>
            void operator()(const Index mag, Eigen::Tensor<ScalarA, 3> &img) const
            {
                Index cx = mag;
                Index cy = mag;

                Eigen::Tensor<Scalar, 3>flow(mag * 2, mag * 2, 2);
                flow.setZero();
                for(Index c  = 0; c < flow.dimension(1); ++c)
                {
                    for(Index r  = 0; r < flow.dimension(0); ++r)
                    {
                        Index dx = c - cx;
                        Index dy = r - cy;
                        if(dx * dx + dy * dy <= mag * mag)
                        {
                            flow(r, c, 0) = dx;
                            flow(r, c, 1) = dy;
                        }
                    }
                }

                ColorMap<Scalar> cmap;
                cmap(flow, img);
            }
        };
    }
}

#endif
