#ifndef NVISION_QR_CODE_DETECTOR_H
#define NVISION_QR_CODE_DETECTOR_H

#include "nvision/src/core/image.h"

namespace nvision
{
    template<typename Scalar>
    struct QRCodeDetection
    {
        Eigen::Matrix<Scalar, 2, 2> boundingBox;
    };

    class QRCodeDetector
    {
    public:
        template<typename Derived>
        QRCodeDetection detect(ImageBase<Derived>& img) const
        {

        }
    };
}

#endif
