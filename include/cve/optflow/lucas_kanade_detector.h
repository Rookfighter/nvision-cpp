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
        typename SmoothFilter = GaussFilter<Scalar, 3>,
        typename GradientFilter = SobelFilter<Scalar>>
    class LucasKanadeDetector
    {
    private:
        SmoothFilter smoothFilter_;
        GradientFilter gradientFilter_;
    public:
        typedef Eigen::Array<Eigen::Matrix<Scalar, 2, 1>, Eigen::Dynamic,
            Eigen::Dynamic> FlowImage;

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

        template<typename Image>
        void apply(const Image &imgA,
            const Image &imgB,
            FlowImage &flowImg) const
        {
            typedef cve::Image<Scalar, Image::Depth> CompImage;

            CompImage gradX(imgA.rows(), imgA.cols());
            CompImage gradY(imgA.rows(), imgA.cols());
            CompImage gradT(imgA.rows(), imgA.cols());
            CompImage gradXX(imgA.rows(), imgA.cols());
            CompImage gradYY(imgA.rows(), imgA.cols());
            CompImage gradXY(imgA.rows(), imgA.cols());
            CompImage gradXT(imgA.rows(), imgA.cols());
            CompImage gradYT(imgA.rows(), imgA.cols());

            gradientFilter_.applyX(imgA, gradX);
            gradientFilter_.applyY(imgA, gradY);

            for(Index c = 0; c < gradT.cols(); ++c)
            {
                for(Index r = 0; r < gradT.rows(); ++r)
                {
                    for(Index d = 0; d < gradT.depth(); ++d)
                    {
                        gradT(r, c, d) = static_cast<Scalar>(imgB(r, c, d)) -
                            static_cast<Scalar>(imgA(r, c, d));
                        gradXX(r, c, d) = gradX(r, c, d) * gradX(r, c, d);
                        gradYY(r, c, d) = gradY(r, c, d) * gradY(r, c, d);
                        gradXY(r, c, d) = gradX(r, c, d) * gradY(r, c, d);
                        gradXT(r, c, d) = gradX(r, c, d) * gradT(r, c, d);
                        gradYT(r, c, d) = gradY(r, c, d) * gradT(r, c, d);
                    }
                }
            }

            CompImage tmpImg;
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

            flowImg.resize(imgA.rows(), imgA.cols());
            for(Index c = 0; c < flowImg.cols(); ++c)
                for(Index r = 0; r < flowImg.rows(); ++r)
                    flowImg(r, c).setZero();

            for(Index c = 0; c < imgA.cols(); ++c)
            {
                for(Index r = 0; r < imgA.rows(); ++r)
                {
                    for(Index d = 0; d < imgA.depth(); ++d)
                    {
                        Eigen::Matrix<Scalar, 2, 2> A;
                        A << gradXX(r, c, d), gradXY(r, c, d),
                            gradXY(r, c, d), gradYY(r, c, d);
                        Eigen::Matrix<Scalar, 2, 1> b;
                        b << -gradXT(r, c, d), -gradYT(r, c, d);
                        flowImg(r, c) += A.inverse() * b;
                    }
                }
            }

            for(Index c = 0; c < flowImg.cols(); ++c)
                for(Index r = 0; r < flowImg.rows(); ++r)
                    flowImg(r, c) /= imgA.depth();

        }
    };
}

#endif
