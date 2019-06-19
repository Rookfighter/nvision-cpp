/*
 * state_saver.h
 *
 * Author: Fabian Meyer
 * Created On: 06 Mar 2019
 */

#ifndef CALIB_STATE_SAVER_H_
#define CALIB_STATE_SAVER_H_

#include <string>
#include <vector>
#include "util/scalar.h"

namespace calib
{
    class StateSaver
    {
    public:
        StateSaver();

        void saveTransforms(const std::string &filename,
            const std::vector<std::string> &names,
            const std::vector<Isometry3> &transforms,
            const Scalar cost) const;

        void saveState(const std::string &filename,
            const std::vector<std::string> &names,
            const Vector &state,
            const Scalar cost) const;
    };
}

#endif
