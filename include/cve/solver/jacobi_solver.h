/* jacobi_solver.h
 *
 * Author: Fabian Meyer
 * Created On: 21 May 2019
 */

#ifndef CVE_JACOBI_SOLVER_H_
#define CVE_JACOBI_SOLVER_H_

#include <vector>
#include <Eigen/SparseCore>

namespace cve
{
    template<typename Scalar>
    class JacobiSolver
    {
    private:
        size_t maxIt_;
        Scalar eps_;
    public:
        typedef Eigen::SparseMatrix<Scalar> SparseMatrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;
        typedef typename Matrix::Index Index;

        JacobiSolver()
            : maxIt_(100), eps_(1e-6)
        { }

        void solve(const SparseMatrix &A,
            const Vector &b,
            Vector &x) const
        {
            x.setZero(A.cols());

            SparseMatrix D(A.rows(), A.cols());
            D.setZero();
            SparseMatrix M = A;

            std::vector<Eigen::Triplet<Scalar>> triplets;
            triplets.reserve(A.rows());
            for(Index i = 0; i < A.rows(); ++i)
            {
                if(A.coeff(i, i) != 0)
                    triplets.push_back({i, i, 1 / A.coeff(i, i)});
            }
            D.setFromTriplets(triplets.begin(), triplets.end());

            triplets.clear();
            for(Index i = 0; i < A.rows(); ++i)
                triplets.push_back({i, i, 0});
            M.setFromTriplets(triplets.begin(), triplets.end());

            Scalar residual = 1e6;
            size_t iterations = 0;
            while(residual >= eps_ * eps_ && (iterations < maxIt_ || maxIt_ == 0))
            {
                x = D * (b - M * x);
                residual = (A * x - b).squaredNorm();
                ++iterations;
            }

        }
    };
}

#endif
