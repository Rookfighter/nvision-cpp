# nvision

![Cpp17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![License](https://img.shields.io/packagist/l/doctrine/orm.svg)

`nvision` is a header-only C++ library for computer vision applications
using the `Eigen3` library.

`nvision` provides:

* image filters like `gauss`, `box`, `laplace`, `sobel` and `scharr`
* feature detectors like ```Harris```, ```ShiTomasi```, ```FAST``` and ```ORB```
* feature descriptors like ```BRIEF``` and ```ORB```
* optical flow estimation using ```LucasKanade``` and ```HornSchunck```
* projective routines for ```triangulation``` and ```camera calibration```
* handling files in  ```pgm```, ```ppm```, ```png``` and ```jpg```format


## Install

`nvision` uses the [build2](https://build2.org/) build system. You can easily use it as dependency by adding it to your `manifest` file.
It will also resolve all additional dependecies of `nvision` for you.

```
depends: libnvision ^1.0.0
```

Otherwise you can simply copy the header files into your project and include the directly.

The core library and all algorithms only require `Eigen3` as dependency. Make sure it can be found by your build system

For loading PNG and JPEG files you will need to link against `libpng` and `libjpg-turbo` additionally.

In Debian based systems you can simply install these dependencies using ```apt-get```.

```bash
apt-get install libeigen3-dev libpng-dev libjpg-turbo-dev
```

## Usage

```nvision``` has many different modules and classes for filtering, feature or
edge detection, optical flow and projective geometry.

Check the `examples/` directory for detailed examples and have a look at the docs for detailed information.

A simple getting started example with a gauss filter:

```cpp
#include <iostream>
#include <nvision/filter.h>
#include <nvision/imageio.h>

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_gauss <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Each value per pixel and depth is representd by
    // a 8-Bit integer.
    nvision::Image<nvision::RGB> src;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    std::cout << "Load " << argv[1] << std::endl;
    nvision::imload(argv[1], src);

    // Create a Gauss filter object. The template parameter determines the
    // internal Scalar type, which is used for computations (e.g. Kernel and
    // Kernel application).
    nvision::GaussFilter<nvision::float32, 9> filter(8.0f);

    // Apply the filter to the source image and store it in dest.
    std::cout << "Apply filter" << std::endl;
    nvision::Image<nvision::RGB> dest = filter(src);

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    std::cout << "Save " << argv[2] << std::endl;
    nvision::imsave(argv[2], dest);

    return 0;
}
```

A simple getting started example with FAST feature detection:

```cpp
/* simple_fast.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <nvision/feature.h>
#include <nvision/draw.h>
#include <nvision/imageio.h>

using namespace nvision;

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_fast <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Each value per pixel and depth is representd by
    // a 8-Bit integer.
    nvision::Image<nvision::RGBf> src;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    std::cout << "Load " << argv[1] << std::endl;
    nvision::imload(argv[1], src);

    std::cout << "Convert to gray scale" << std::endl;
    nvision::Image<nvision::Grayf> gray = nvision::image::convert<Grayf>(src);

    // Create a FAST feature detector object. The template parameter determines the
    // internal Scalar type, which is used for computations and feature point
    // representation.
    nvision::FASTFeature<nvision::float32> detector;

    // Extract feature points from the image.
    using FeatureMatrix = typename nvision::FASTFeature<nvision::float32>::FeatureMatrix;
    FeatureMatrix keypoints;
    std::cout << "Detect features" << std::endl;
    detector(gray, keypoints);

    std::cout << "Draw " << keypoints.cols() << " keypoints" << std::endl;
    nvision::image::draw(src, keypoints, nvision::Marker::Circle, nvision::color::red<RGBf>());

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    std::cout << "Save " << argv[2] << std::endl;
    nvision::imsave(argv[2], src);

    return 0;

    return 0;
}
```

## Compile

`nvision` can be compiled using the build2 build system.

```
bdep init -C @gcc-debug cc config.config.load=build-config/gcc.debug.build
b
```

This will resolve all dependencies of `nvision` and build all examples and unit tests with your default compiler.
You can run the unit tests using build2 again.

```
b test
```
