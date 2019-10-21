# cv-eigen

![Cpp11](https://img.shields.io/badge/C%2B%2B-11-blue.svg)
![License](https://img.shields.io/packagist/l/doctrine/orm.svg)
![Travis Status](https://travis-ci.org/Rookfighter/cv-eigen.svg?branch=master)
![Appveyer Status](https://ci.appveyor.com/api/projects/status/oskru1t4q55s4uxa?svg=true)

```cv-eigen``` is a header-only C++ library for computer vision applications
using the ```Eigen3``` library and its ```Tensor``` module.

```cv-eigen``` provides:

* image filters like ```gauss```, ```box```, ```laplace```, ```sobel``` and ```scharr```
* feature detectors like ```Harris```, ```ShiTomasi```, ```FAST``` and ```ORB```
* feature descriptors like ```BRIEF``` and ```ORB```
* optical flow estimation using ```LucasKanade``` and ```HornSchunck```
* projective routines for ```triangulation``` and ```camera calibration```
* handling files in  ```pgm```, ```ppm```, ```png``` and ```jpg```format


## Install

Simply copy the header files into your project or install them using
the CMake build system by typing

```bash
cd path/to/repo
mkdir build
cd build
cmake ..
make install
```

The library requires ```Eigen3``` to be installed on your system.

Moreover You have to link your application against ```libpng``` and
```libjpg``` for handling ```png``` and ```jpg``` files.

In Debian based systems you can simply install these dependencies using ```apt-get```.

```bash
apt-get install libeigen3-dev libpng-dev libjpg-dev
```

Make sure ```Eigen3``` can be found by your build system.

You can use the CMake Find module in ```cmake/``` to find the installed headers.

## Usage

```cv-eigen``` has many different modules and classes for filtering, feature or
edge detection, optical flow and projective geometry.

Please see the docs, examples and the source files for detailed information.

A simple getting started example with a gauss filter:

```cpp
#include <iostream>
#include <cve/filter/gauss_filter.h>
#include <cve/imageio/imageio.h>
#include <cve/core/image.h>

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_gauss <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Declare Image tensors. Each value per pixel and depth is representd by
    // a 8-Bit integer.
    // There are also Image16, Image32, Imagef or Imaged available for
    // 16-bit, 32-bit, float and double types respectively-
    cve::Image8 src;
    cve::Image8 dest;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    // Currently PPM PGM, PNG and JPG are supported.
    cve::imload(argv[1], src);

    // Create a Gauss filter object. The template parameter determines the
    // internal Scalar type, which is used for computations (e.g. Kernel and
    // Kernel application).
    // This constructor expects the standard deviation of the gaussian function
    // and calculates the kernel size automatically.
    cve::GaussFilter<float> gaussFilter(3);

    // Apply the Gauss filter to the source image and store it in dest.
    gaussFilter(src, dest);

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    // Currently PPM PGM, PNG and JPG are supported.
    cve::imsave(argv[2], dest);
}
```

A simple getting started example with FAST feature detection:

```cpp
#include <iostream>
#include <cve/feature/fast_detector.h>
#include <cve/draw/shape_drawer.h>
#include <cve/draw/colors.h>
#include <cve/imageio/imageio.h>

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: simple_fast <in-file> <out-file>" << std::endl;
        return -1;
    }

    // Declare Image tensors. Each value per pixel and depth is representd by
    // a float.
    // There are also Image8, Image16, Image32 or Imaged available for
    // 8-bit, 16-bit, 32-bit and double types respectively-
    cve::Imagef src;
    cve::Imagef dest;

    cve::Matrixf keypoints;

    // Load the image from a file. The file type is determined by the extension
    // of the file.
    // Currently PPM PGM, PNG and JPG are supported.
    cve::imload(argv[1], src);

    // Create a fast feature object. The template parameter determines the
    // internal Scalar type, which is used for computations.
    // This constructor expects the standard deviation of the gaussian function
    // and calculates the kernel size automatically.
    cve::FASTDetector<float> fast;

    // Apply the FAST detector to the source image and stores its keypoints.
    fast(src, keypoints);

    // Create an output image with the detected keypoints.
    // Copy input image to output image.
    dest = src;
    // Draw circles of radius of 10 around the keypoints
    cve::ShapeDrawer<float> drawer;
    drawer.drawCircle(keypoints, 10, cve::Colorf::Red(), dest);

    // Save the image to a file. The file type is determined by the extension
    // of the file.
    // Currently PPM PGM, PNG and JPG are supported.
    cve::imsave(argv[2], dest);
}
```
