/* eigen_require.h
 *
 *  Created on: 27 Jun 2019
 *      Author: Fabian Meyer
 */

#ifndef CVE_EIGEN_REQUIRE_H_
#define CVE_EIGEN_REQUIRE_H_

#include <catch.hpp>
#include <cve/image.h>

#define REQUIRE_MATRIX_APPROX(a, b, eps) do {                                 \
        REQUIRE(a.cols() == b.cols());                                        \
        REQUIRE(a.rows() == b.rows());                                        \
        for(Index _c = 0; _c < a.cols(); ++_c)                                \
            for(Index _r = 0; _r < a.rows(); ++_r)                            \
                REQUIRE(Approx(a(_r, _c)).margin(eps) == b(_r, _c));          \
    } while(0)

#define REQUIRE_MATRIX(a, b) do {                                             \
        REQUIRE(a.cols() == b.cols());                                        \
        REQUIRE(a.rows() == b.rows());                                        \
        for(Index _c = 0; _c < a.cols(); ++_c)                                \
            for(Index _r = 0; _r < a.rows(); ++_r)                            \
                REQUIRE(a(_r, _c) == b(_r, _c));                              \
    } while(0)

#define REQUIRE_IMAGE_APPROX(a, b, eps) do {                                  \
        REQUIRE(a.dimensions().size() == 3);                                  \
        REQUIRE(b.dimensions().size() == 3);                                  \
        REQUIRE(a.dimension(0) == b.dimension(0));                            \
        REQUIRE(a.dimension(1) == b.dimension(1));                            \
        REQUIRE(a.dimension(2) == b.dimension(2));                            \
        for(Index _d = 0; _d < a.dimension(2); ++_d)                          \
            for(Index _c = 0; _c < a.dimension(1); ++_c)                      \
                for(Index _r = 0; _r < a.dimension(0); ++_r)                  \
                    REQUIRE(Approx(a(_r, _c, _d)).margin(eps) ==              \
                        b(_r, _c, _d));                                       \
    } while(0)

#define REQUIRE_IMAGE(a, b) do {                                              \
        REQUIRE(a.dimensions().size() == 3);                                  \
        REQUIRE(b.dimensions().size() == 3);                                  \
        REQUIRE(a.dimension(0) == b.dimension(0));                            \
        REQUIRE(a.dimension(1) == b.dimension(1));                            \
        REQUIRE(a.dimension(2) == b.dimension(2));                            \
        for(Index _d = 0; _d < a.dimension(2); ++_d)                          \
            for(Index _c = 0; _c < a.dimension(1); ++_c)                      \
                for(Index _r = 0; _r < a.dimension(0); ++_r)                  \
                    REQUIRE(a(_r, _c, _d) == b(_r, _c, _d));                  \
    } while(0)

#endif
