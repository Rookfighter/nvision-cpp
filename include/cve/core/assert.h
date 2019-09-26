/* assert.h
 *
 * Author: Fabian Meyer
 * Created On: 26 Sep 2019
 */

#ifndef CVE_ASSERT_H_
#define CVE_ASSERT_H_

#include <cstdio>

#ifdef CVE_NDEBUG
#define CVE_ASSERTM(e, m, ...) ((void)0)
#else
#define CVE_ASSERTM(e, m, ...)                                              \
    do {                                                                    \
        if(!(e))                                                            \
        {                                                                   \
            fprintf(stderr, "%s:%d: Assertion \"%s\" failed. " m, __FILE__, \
                __LINE__, #e,  ##__VA_ARGS__);                              \
            abort();                                                        \
        }                                                                   \
    } while(0)
#endif

#define CVE_ASSERT(e) CVE_ASSERTM(e, "")

#endif
