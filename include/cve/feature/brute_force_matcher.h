/* brute_force_matcher.h
 *
 * Author: Fabian Meyer
 * Created On: 21 Jun 2019
 */

#ifndef CVE_BRUTE_FORCE_MATCHER_H_
#define CVE_BRUTE_FORCE_MATCHER_H_

namespace cve
{
    template<typename Scalar>
    struct HammingDistanceBitwise
    {
        Scalar operator()(const uint8_t lhs, const uint8_t rhs) const
        {
            uint8_t dist = 0;
            for(uint8_t val = lhs ^ rhs; val > 0; val >>= 1)
            {
                if(val & 0x01 == 0x01)
                    ++dist;
            }

            return static_cast<Scalar>(dist);
        };
    };

    template<typename Scalar, typename Distance>
    class BruteForceMatcher
    {

    };
}

#endif
