# Quantize
A utility for quantizing an image using the Gervautz-Purgathofer octree method:
* [Wicked Code -- MSJ, August 1996](https://www.microsoft.com/msj/archive/S3F1.aspx)
* [Wicked Code, MSJ October 1997](http://www.microsoft.com/msj/1097/wicked1097.aspx)
* [Color Quantization using Octrees | Dr Dobb's](http://www.drdobbs.com/cpp/color-quantization-using-octrees/184409805)
* [Optimizing Color Quantization for ASP.NET Images](https://msdn.microsoft.com/en-us/library/aa479306.aspx)

# Build Instructions
1. Install [CMake](https://cmake.org/) 3.1.0 or later
2. Install [Boost](http://www.boost.org/)
3. Install [OpenImageIO](http://openimageio.org/)
4. Clone a copy of the repository:
    ```
    git clone https://github.com/SimonBoorer/Quantize.git
    ```
5. Build with CMake:
    ```
    cd Quantize
    mkdir build
    cd build
    cmake ..
    ```
 
# Usage
```
Usage: Quantize [options] input-file output-file
Allowed options:
  -h [ --help ]                   produce help message
  -m [ --max-colours ] arg (=255) maximum number of colours in the palette
  -b [ --colour-bits ] arg (=8)   number of significant bits in each 8-bit 
                                  colour compartment
```

