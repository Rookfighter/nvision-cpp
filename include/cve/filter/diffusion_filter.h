/* diffusion_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 05 Sep 2019
 */

#ifndef CVE_DIFFUSION_FILTER_H_
#define CVE_DIFFUSION_FILTER_H_

#include "cve/filter/forward_differences_filter.h"
#include "cve/filter/backward_differences_filter.h"
#include "cve/filter/central_differences_filter.h"
#include "cve/filter/sobel_filter.h"

namespace cve
{
    template<typename Scalar>
    class GaussianDiffusivity
    {
    private:
        Scalar lambda_;
    public:
        GaussianDiffusivity()
            : GaussianDiffusivity(30)
        { }

        GaussianDiffusivity(const Scalar lambda)
            : lambda_(lambda)
        { }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            destImg = (-srcImg / srcImg.constant(lambda_ * lambda_)).exp();
        }
    };

    template<typename Scalar>
    class TotalVariationDiffusivity
    {
    public:
        TotalVariationDiffusivity()
        { }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            destImg = srcImg.constant(1) / srcImg.sqrt();
        }
    };

    template<typename Scalar,
        typename Diffusivity=GaussianDiffusivity<Scalar>>
    class DiffusionFilter
    {
    private:
        Diffusivity diffusivity_;
        Index maxIt_;
    public:
        DiffusionFilter()
            : DiffusionFilter(10)
        { }

        DiffusionFilter(const Index iterations)
            : diffusivity_(), maxIt_(iterations)
        { }

        void setMaxIterations(const Index iterations)
        {
            maxIt_ = iterations;
        }

        void setDiffusivity(const Diffusivity &diffusivity)
        {
            diffusivity_ = diffusivity;
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Tensor<ScalarB, 3> u = srcImg;
            Eigen::Tensor<ScalarB, 3> uxFwd;
            Eigen::Tensor<ScalarB, 3> uyFwd;
            Eigen::Tensor<ScalarB, 3> uxBwd;
            Eigen::Tensor<ScalarB, 3> uyBwd;
            Eigen::Tensor<ScalarB, 3> uxCtl;
            Eigen::Tensor<ScalarB, 3> uyCtl;
            Eigen::Tensor<ScalarB, 3> uMag;

            Eigen::Tensor<ScalarB, 3> g;
            // Eigen::Tensor<ScalarB, 3> gxFwd;
            // Eigen::Tensor<ScalarB, 3> gyFwd;
            // Eigen::Tensor<ScalarB, 3> gxBwd;
            // Eigen::Tensor<ScalarB, 3> gyBwd;

            ForwardDifferencesFilter<Scalar> fwdFilter;
            ForwardDifferencesFilter<Scalar> bwdFilter;
            CentralDifferencesFilter<Scalar> ctlFilter;

            for(Index i = 0; i < maxIt_; ++i)
            {
                fwdFilter(u, uxFwd, uyFwd);
                bwdFilter(u, uxBwd, uyBwd);
                ctlFilter(u, uxCtl, uyCtl);

                uxCtl /= uxCtl.constant(2);
                uyCtl /= uyCtl.constant(2);
                image::magnitude(uxCtl, uyCtl, uMag);
                diffusivity_(uMag, g);

                // fwdFilter(g, gxFwd, gyFwd);
                // bwdFilter(g, gxBwd, gyBwd);
                // gxFwd /= gxFwd.constant(2);
                // gyFwd /= gyFwd.constant(2);
                // gxBwd /= gxBwd.constant(2);
                // gyBwd /= gyBwd.constant(2);

                u += (g * uxFwd - g * uxBwd) + (g * uyFwd - g * uyBwd);
            }

            destImg = u;
        }
    };
}

#endif
