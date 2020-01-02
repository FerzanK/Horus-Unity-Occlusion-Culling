# Horus - Occlusion culling for Unity

* Occlusion culling increases rendering performance simply by not rendering geometry that is outside the view frustum or hidden by objects closer to the camera. Horus is a non-conservative occlusion culler and it performs better than Umbra (Unity's native occlusion culler) in small scenes such as interiors filled with lots of objects.

* **Horus is currently in alpha stage!**

<p align="center">
<img src="https://github.com/FerzanK/Horus-Unity-Occlusion-Culling/blob/master/Documentation/VisibilityAnimation.gif"/>
</p>

## Features
* Written in C++17
* Works better than Umbra in small spaces
* Fully dynamic, baking not required

## Getting Started

* Add HorusManager.cs located in UnityScript folder to your scene and attach it to your camera
* Compile plugin with Visual Studio 2019, name DLL file horus.dll and place it in Plugins folder of your Unity project


### Todo

* Unit tests and CI setup
* Provide pre-compiled binaries
* Custom memory allocator & memory pool
* Mesh -> Octree converter
* BVH/Octree based pre-culling
* Fiber based task system
* Non-static mesh support
* Transparent object support
* Skinned mesh support
* Better C# marshalling code
* Multicore support

## Versioning

I use [SemVer](http://semver.org/) for versioning. Current version is 0.1

## Built With

* [OpenGL Mathematics (GLM)](https://github.com/g-truc/glm) - C++ Math library
* [spdlog](https://github.com/gabime/spdlog) - C++ logging library
* [Bitmap](https://github.com/ArashPartow/bitmap) - C++ Bitmap Library

## References

* **Real-Time Rendering, Fourth Edition** http://www.realtimerendering.com/
* **Fixing Frustum Culling**  http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
* **Efficient Occlusion Culling** https://developer.download.nvidia.com/books/HTML/gpugems/gpugems_ch29.html
* **Parallelizing the Naughty Dog Engine Using Fibers**  https://www.gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
