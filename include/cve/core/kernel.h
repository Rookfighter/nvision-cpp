/* kernel.h
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#ifndef CVE_KERNEL_H_
#define CVE_KERNEL_H_

#include "cve/core/border_handling.h"

namespace cve
{
    namespace kernel
    {
        template<typename ScalarA,
            typename ScalarB,
            typename DerivedC,
            typename BorderHandling>
        inline void apply(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg,
            const Eigen::MatrixBase<DerivedC> &kernel,
            const BorderHandling &handling)
        {
            destImg.resize(srcImg.dimensions());
            destImg.setZero();

            for(Index d = 0; d < srcImg.dimension(2); ++d)
            {
                for(Index c = 0; c < srcImg.dimension(1); ++c)
                {
                    for(Index r = 0; r < srcImg.dimension(0); ++r)
                    {
                        for(Index kcol = 0; kcol < kernel.cols(); ++kcol)
                        {
                            Index offsetCol = kcol - kernel.cols() / 2;

                            for(Index krow = 0; krow < kernel.rows(); ++krow)
                            {
                                Index offsetRow = krow - kernel.rows() / 2;
                                ScalarA imgval = handling(srcImg, r + offsetRow, c + offsetCol, d);

                                destImg(r, c, d) += static_cast<ScalarB>(kernel(krow, kcol) * imgval);
                            }
                        }
                    }
                }
            }
        }
    }
}

#endif
