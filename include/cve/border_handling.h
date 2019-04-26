/* border_handling.h
 *
 * Author: Fabian Meyer
 * Created On: 26 Apr 2019
 */

#ifndef CVE_BORDER_HANDLING_H_
#define CVE_BORDER_HANDLING_H_

#include "cve/image.h"

namespace cve
{
    enum class BorderHandling
    {
        Repeat,
        Reflect
    };

    namespace border
    {
        inline Index reflect(const Index idx, const Index min, const Index max)
        {
            if(idx < min)
                return min + std::abs(idx);
            else if(idx >= max)
                return 2 * max - std::abs(idx) - 1;
            else
                return idx;
        }

        inline Index repeat(const Index idx, const Index min, const Index max)
        {
            if(idx < min)
                return min;
            else if (idx >= max)
                return max - 1;
            else
                return idx;
        }

        inline Index handle(const Index idx, const Index min, const Index max,
            const BorderHandling handling)
        {
            switch(handling)
            {
            case BorderHandling::Repeat:
                return repeat(idx, min, max);
            case BorderHandling::Reflect:
                return reflect(idx, min, max);
            default:
                assert(false);
                return 0;
            }
        }
    }
}

#endif
