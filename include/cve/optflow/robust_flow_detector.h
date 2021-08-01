/* robust_flow_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 06 Sep 2019
 */

#ifndef CVE_ROBUST_FLOW_DETECTOR_H_
#define CVE_ROBUST_FLOW_DETECTOR_H_

#include "cve/filter/box_filter.h"
#include "cve/filter/central_differences_filter.h"
#include "cve/filter/forward_differences_filter.h"
#include "cve/filter/sobel_filter.h"
#include "cve/core/penalizer_functors.h"

namespace cve
{
    template<typename Scalar,
        typename Penalizer=TotalVariationPenalizer<Scalar>,
        typename GradientFilter=CentralDifferencesFilter<Scalar>,
        typename BorderHandling=BorderReflect>
    class RobustFlowDetector
    {
    private:
        Index maxIt_;
        Scalar alpha_;
        Scalar gamma_;
        GradientFilter gradientFilter_;
        Penalizer penalizer_;
        BorderHandling handling_;
    public:
        RobustFlowDetector()
            : RobustFlowDetector(10, 1, 1)
        { }

        RobustFlowDetector(const Index iterations, const Scalar alpha, const Scalar gamma)
            : maxIt_(iterations), alpha_(alpha), gamma_(gamma),
            gradientFilter_(), penalizer_(), handling_()
        { }

        void setMaxIterations(const Index iterations)
        {
            maxIt_ = iterations;
        }

        void setSmoothingConstant(const Scalar alpha)
        {
            alpha_ = alpha;
        }

        void setGradientConstant(const Scalar gamma)
        {
            gamma_ = gamma;
        }

        void setGradientFilter(const GradientFilter &filter)
        {
            gradientFilter_ = filter;
        }

        void setPenalizer(const Penalizer &penalizer)
        {
            penalizer_ = penalizer;
        }

        template<typename ScalarA>
        void operator()(const Eigen::Tensor<ScalarA, 3> &imgA,
            const Eigen::Tensor<ScalarA, 3> &imgB,
            Eigen::Tensor<Scalar, 3> &flowImg) const
        {
            assert(imgA.dimension(2) == 1);
            assert(imgA.dimension(0) == imgB.dimension(0));
            assert(imgA.dimension(1) == imgB.dimension(1));
            assert(imgA.dimension(2) == imgB.dimension(2));

            Index height = imgA.dimension(0);
            Index width = imgA.dimension(1);

            Eigen::Tensor<Scalar, 3> Ix;
            Eigen::Tensor<Scalar, 3> Iy;
            Eigen::Tensor<Scalar, 3> IxB;
            Eigen::Tensor<Scalar, 3> IyB;
            Eigen::Tensor<Scalar, 3> Ixx;
            Eigen::Tensor<Scalar, 3> Iyy;
            Eigen::Tensor<Scalar, 3> Ixy;
            Eigen::Tensor<Scalar, 3> It;
            Eigen::Tensor<Scalar, 3> Ixt;
            Eigen::Tensor<Scalar, 3> Iyt;

            Eigen::Tensor<Scalar, 3> u(height, width, 1);
            Eigen::Tensor<Scalar, 3> v(height, width, 1);
            Eigen::Tensor<Scalar, 3> uavg;
            Eigen::Tensor<Scalar, 3> vavg;
            Eigen::Tensor<Scalar, 3> uprev;
            Eigen::Tensor<Scalar, 3> vprev;

            Eigen::Tensor<Scalar, 3> ux;
            Eigen::Tensor<Scalar, 3> uy;
            Eigen::Tensor<Scalar, 3> vx;
            Eigen::Tensor<Scalar, 3> vy;

            Eigen::Tensor<Scalar, 3> a;
            Eigen::Tensor<Scalar, 3> b;
            Eigen::Tensor<Scalar, 3> c;

            Eigen::Tensor<Scalar, 3> dataTerm;
            Eigen::Tensor<Scalar, 3> gradTermX;
            Eigen::Tensor<Scalar, 3> gradTermY;

            gradientFilter_(imgA, Ix, Iy);
            gradientFilter_(imgB, IxB, IyB);
            gradientFilter_(Ix, Ixx, Ixy);
            gradientFilter_(Iy, Ixy, Iyy);

            It = imgB.template cast<Scalar>() - imgA.template cast<Scalar>();
            Ixt = IxB - Ix;
            Iyt = IyB - Iy;

            u.setZero();
            v.setZero();

            Eigen::Matrix<Scalar, 3, 3> kernelmat;
            kernelmat << 0, 1, 0,
                      1, 0, 1,
                      0, 1, 0;
            kernelmat /= 4;

            for(Index i = 0; i < maxIt_; ++i)
            {
                gradientFilter_(u, ux, uy);
                gradientFilter_(v, vx, vy);

                dataTerm = Ix * u + Iy * v + It;
                a = (dataTerm * dataTerm).unaryExpr(penalizer_);

                b = (ux * ux + uy * uy + vx * vx + vy * vy).unaryExpr(penalizer_);

                gradTermX = Ixx * u + Ixy * v + Ixt;
                gradTermY = Ixy * u + Iyy * v + Iyt;
                c = (gradTermX * gradTermX + gradTermY * gradTermY).unaryExpr(penalizer_);

                kernel::apply(u, uavg, kernelmat, handling_);
                kernel::apply(v, vavg, kernelmat, handling_);

                uprev = u;
                vprev = v;

                u = (a * Ix * It + gamma_ * c * (Ixt * Ixx + Iyt * Ixy)
                    + alpha_ * b * uavg
                    - v * (a * Iy * Ix + gamma_ * c * Ixy * (Ixx + Iyy)))
                    / (a * Ix * Ix + 4 * alpha_ * b + gamma_ * c * (Ixx * Ixx + Ixy * Ixy));

                v = (a * Iy * It + gamma_ * c * (Ixt * Ixy + Iyt * Iyy)
                    + alpha_ * b * vavg
                    - u * (a * Ix * Iy + gamma_ * c * Ixy * (Ixx + Iyy)))
                    / (a * Iy * Iy + 4 * alpha_ * b + gamma_ * c * (Iyy * Iyy + Ixy * Ixy));
            }

            flowImg.resize(height, width, 2);
            for(Index c = 0; c < width; ++c)
            {
                for(Index r = 0; r < height; ++r)
                {
                    flowImg(r, c, 0) = -u(r, c, 0);
                    flowImg(r, c, 1) = -v(r, c, 0);
                }
            }
        }
    };
}

#endif
