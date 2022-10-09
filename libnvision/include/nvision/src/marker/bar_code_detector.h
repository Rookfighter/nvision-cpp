#ifndef NVISION_BAR_CODE_DETECTOR_H
#define NVISION_BAR_CODE_DETECTOR_H

#include "nvision/src/core/image.h"

namespace nvision
{
    template<typename Scalar>
    struct BarcodeDetection
    {
        Eigen::Matrix<Scalar, 2, 2> boundingBox;
    };

    class BarcodeDetector
    {
    public:
        template<typename Derived>
        BarcodeDetection detect(ImageBase<Derived>& img) const
        {

        }
    };
}

#endif
