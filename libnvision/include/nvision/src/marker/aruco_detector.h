#ifndef NVISION_ARUCO_DETECTOR_H
#define NVISION_ARUCO_DETECTOR_H

#include "nvision/src/core/image.h"

namespace nvision
{
    template<typename Scalar>
    struct ArucoDetection
    {
        Index id;
        Eigen::Matrix<Scalar, 2, 2> boundingBox;
    };

    class ArucoDetector
    {
    public:
        template<typename Derived>
        QRCodeDetection detect(ImageBase<Derived>& img) const
        {

        }
    };
}

#endif
