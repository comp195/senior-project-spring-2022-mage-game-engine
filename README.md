# MAGE Game Engine

_MAGE_ (MAGE Amazing Game Engine) is the final senior project of Jordan Scharkey. It is a Vulkan-based, 3D game engine written primarily in C++.

## Dependencies and Compiling

#### Vulkan, GLFW, and glslc

Vulkan drivers for whichever operating system you are using will be required. You can find out more regarding this [here.](https://www.vulkan.org/tools#vulkan-gpu-resources) You will also need GLFW, a library that helps with window creation. You can read about that, along with additional dependency setup, [here.](https://vulkan-tutorial.com/Development_environment) We will be using the shader compiler [glslc](https://github.com/google/shaderc/blob/main/downloads.md), which will also be required as a dependency.

#### Compiling Shaders

Use the script for your respective operating system to compile shaders before running code. Note: the .bat file for Windows users currently requires manual input of glslc.exe location on personal system, but I will look into fixing this in the future.

#### Using CMake to Build

I've chosen to compile with CMake, as it offers better cross-platform compatability. After cloning the repository, you can create a "mage-game-engine" executable by generating a Makefile using CMake.

On Linux, this can be achieved by using the following commands:

` > cd /path/to/cloned/repository/ `

` > cmake .`

` > make `

After that, simply run the executable with:

` > ./mage-game-engine `

## TODO

I hope to achieve the following milestones before my Senior Project Day:

* ~~Create a window~~
* Render a simple triangle
* Render a cube
* Add motion to rendered objects
* Respond to user input
* Add a simple lighting system
* Create a basic tutorial displaying the above features

I also hope to achieve this with as much cross-platform support as possible, but seeing as I'm developing on an NVIDIA-driven Linux machine, these particular systems will recieve precidence if time becomes an issue.

## Contact

You can reach out to me at my university email, [j_scharkey@u.pacific.edu.](mailto:j_scharkey@u.pacific.edu)
