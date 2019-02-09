/* test_kernel.cpp
 *
 * Author: Fabian Meyer
 * Created On: 09 Feb 2019
 */

#include <catch.hpp>
#include <cve/image.h>
#include <cve/kernel.h>

TEST_CASE("Kernel")
{
    cve::GrayImage img(100, 100);

    cve::Kernel<double, 3, 3> kernel;
}
