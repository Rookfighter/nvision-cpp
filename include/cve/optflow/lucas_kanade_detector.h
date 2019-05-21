/* lucas_kanade_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 16 May 2019
 */

#ifndef CVE_LUCAS_KANADE_DETECTOR_H_
#define CVE_LUCAS_KANADE_DETECTOR_H_

#include "cve/filter/gauss_filter.h"
#include "cve/filter/sobel_filter.h"

namespace cve
{
    template<typename Scalar,
        typename SmoothFilter = GaussFilter<Scalar, 5>,
        typename GradientFilter = SobelFilter<Scalar>>
    class LucasKanadeDetector
    {
    private:
        SmoothFilter smoothFilter_;
        GradientFilter gradientFilter_;
    public:

        LucasKanadeDetector()
            : smoothFilter_(), gradientFilter_()
        {
        }

        void setSmoothFilter(const SmoothFilter &filter)
        {
            smoothFilter_ = filter;
        }

        void setGradientFilter(const GradientFilter &filter)
        {
            gradientFilter_ = filter;
        }

        template<typename ScalarA>
        void apply(const Eigen::Tensor<ScalarA, 3> &imgA,
            const Eigen::Tensor<ScalarA, 3> &imgB,
            Eigen::Tensor<Scalar, 3> &flowImg) const
        {
            Eigen::Tensor<Scalar, 3> gradX(imgA.dimensions());
            Eigen::Tensor<Scalar, 3> gradY(imgA.dimensions());
            Eigen::Tensor<Scalar, 3> gradT(imgA.dimensions());
            Eigen::Tensor<Scalar, 3> gradXX(imgA.dimensions());
            Eigen::Tensor<Scalar, 3> gradYY(imgA.dimensions());
            Eigen::Tensor<Scalar, 3> gradXY(imgA.dimensions());
            Eigen::Tensor<Scalar, 3> gradXT(imgA.dimensions());
            Eigen::Tensor<Scalar, 3> gradYT(imgA.dimensions());

            gradientFilter_.applyX(imgA, gradX);
            gradientFilter_.applyY(imgA, gradY);

            gradT = imgB.template cast<Scalar>() - imgA.template cast<Scalar>();
            gradXX = gradX * gradX;
            gradYY = gradY * gradY;
            gradXY = gradX * gradY;
            gradXT = gradX * gradT;
            gradYT = gradY * gradT;

            Eigen::Tensor<Scalar, 3> tmpImg;
            smoothFilter_.apply(gradXX, tmpImg);
            gradXX = tmpImg;
            smoothFilter_.apply(gradYY, tmpImg);
            gradYY = tmpImg;
            smoothFilter_.apply(gradXY, tmpImg);
            gradXY = tmpImg;
            smoothFilter_.apply(gradXT, tmpImg);
            gradXT = tmpImg;
            smoothFilter_.apply(gradYT, tmpImg);
            gradYT = tmpImg;

            flowImg.resize(imgA.dimension(0), imgA.dimension(1), 2);
            flowImg.setZero();
            for(Index d = 0; d < imgA.dimension(2); ++d)
            {
                for(Index c = 0; c < imgA.dimension(1); ++c)
                {
                    for(Index r = 0; r < imgA.dimension(0); ++r)
                    {
                        Eigen::Matrix<Scalar, 2, 2> A;
                        A << gradXX(r, c, d), gradXY(r, c, d),
                            gradXY(r, c, d), gradYY(r, c, d);
                        Eigen::Matrix<Scalar, 2, 1> b;
                        b << -gradXT(r, c, d), -gradYT(r, c, d);
                        Eigen::Matrix<Scalar, 2, 1> flow = (A.inverse() * b).array();
                        flowImg(r, c, 0) += flow(0);
                        flowImg(r, c, 1) += flow(1);
                    }
                }
            }

            flowImg /= flowImg.constant(imgA.dimension(2));
        }
    };
}

#endif
