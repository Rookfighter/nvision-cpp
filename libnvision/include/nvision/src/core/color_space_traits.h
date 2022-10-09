/// @author Fabian Meyer
/// @date 09 Dec 2019
/// @file


#ifndef NVISION_COLOR_SPACE_TRAITS_H_
#define NVISION_COLOR_SPACE_TRAITS_H_

namespace nvision
{
    /// Type trait which determines if a given type is a color space or not.
    template<typename T>
    struct IsColorSpace
    {
        static constexpr bool value = false;
    };

    template<typename T>
    struct GetIntegralColorSpace
    { };
}

#endif
