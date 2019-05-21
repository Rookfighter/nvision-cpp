/* horn_schunck_detector.h
 *
 * Author: Fabian Meyer
 * Created On: 18 May 2019
 */

#ifndef CVE_HORN_SCHUNCK_DETECTOR_H_
#define CVE_HORN_SCHUNCK_DETECTOR_H_

#include "cve/filter/gauss_filter.h"
#include "cve/filter/sobel_filter.h"
#include "cve/solver/jacobi_solver.h"

namespace cve
{
    template<typename Scalar,
        typename GradientFilter = SobelFilter<Scalar>,
        typename SparseSolver = JacobiSolver<Scalar>>
    class HornSchunckDetector
    {
    private:
        Scalar alpha_;
        GradientFilter gradientFilter_;
        SparseSolver solver_;
    public:
        typedef Eigen::SparseMatrix<Scalar> SparseMatrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;

        HornSchunckDetector()
            : HornSchunckDetector(200)
        {
        }

        HornSchunckDetector(const Scalar alpha)
            : alpha_(alpha), gradientFilter_(), solver_()
        {
        }

        void setRegularizationConstant(const Scalar alpha)
        {
            alpha = alpha_;
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
            assert(imgA.dimension(0) == imgB.dimension(0));
            assert(imgA.dimension(1) == imgB.dimension(1));
            assert(imgA.dimension(2) == imgB.dimension(2));

            Index height = imgA.dimension(0);
            Index width = imgA.dimension(1);
            Index pixelCnt = width * height;
            Scalar alphaInv = 1 / alpha_;

            Eigen::Tensor<Scalar, 3> gradX;
            Eigen::Tensor<Scalar, 3> gradY;
            Eigen::Tensor<Scalar, 3> gradT;

            gradientFilter_.applyX(imgA, gradX);
            gradientFilter_.applyY(imgA, gradY);
            gradT = (imgB - imgA).template cast<Scalar>();

            Vector b(2 * pixelCnt);
            std::vector<Eigen::Triplet<Scalar>> triplets;
            triplets.reserve(pixelCnt * 12);

            // add main diagonal elements on first half
            for(Index i = 0; i < pixelCnt; ++i)
            {
                // calculate image cooridnates
                Index x = i / width;
                Index y = i % width;
                // calculate smoothness factor by neighbour count
                Scalar smooth = 0;
                if(x > 0)
                    smooth -= 1;
                if(x < width-1)
                    smooth -= 1;
                if(y > 0)
                    smooth -= 1;
                if(y < height-1)
                    smooth -= 1;

                // add main diagonal first half
                Scalar val = -alphaInv + gradX(y, x, 0) * gradX(y, x, 0) + smooth;
                triplets.push_back({i, i, val});
                // add main diagonal second half
                val = -alphaInv + gradY(y, x, 0) * gradY(y, x, 0) + smooth;
                triplets.push_back({pixelCnt + i, pixelCnt + i, val});

                // near diagonals
                if(i > 0 && i < width-1)
                {
                    triplets.push_back({i, i+1, 1});
                    triplets.push_back({i+1, i, 1});
                    triplets.push_back({pixelCnt + i, pixelCnt + i+1, 1});
                    triplets.push_back({pixelCnt + i+1, pixelCnt + i, 1});
                }

                // far diagonals
                if(i >= width && i < pixelCnt - width)
                {
                    triplets.push_back({i, i+width, 1});
                    triplets.push_back({i+width, i, 1});
                    triplets.push_back({pixelCnt + i, pixelCnt + i + width, 1});
                    triplets.push_back({pixelCnt + i + width, pixelCnt + i, 1});
                }

                // set block diagonals
                val = -alphaInv + gradX(y, x, 0) * gradY(y, x, 0);
                triplets.push_back({i, pixelCnt + i, val});
                triplets.push_back({pixelCnt + i, i, val});

                b(i) = alphaInv * gradX(y, x, 0) * gradT(y, x, 0);
                b(i+pixelCnt) = alphaInv * gradY(y, x, 0) * gradT(y, x, 0);
            }

            SparseMatrix A(2 * pixelCnt, 2 * pixelCnt);
            A.setFromTriplets(triplets.begin(), triplets.end());

            Vector flowVec;
            solver_.solve(A, b, flowVec);

            flowImg.resize(imgA.dimension(0), imgA.dimension(1), 2);
            for(Index i = 0; i < pixelCnt; ++i)
            {
                // calculate image cooridnates
                Index x = i / width;
                Index y = i % width;

                flowImg(y, x, 0) = flowVec(2*i);
                flowImg(y, x, 1) = flowVec(2*i+1);
            }
        }
    };
}

#endif
