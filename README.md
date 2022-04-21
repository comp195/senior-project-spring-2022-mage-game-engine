# MAGE Game Engine

_MAGE_ (MAGE Amazing Game Engine) is the final senior project of Jordan Scharkey. It is a Vulkan-based, 3D game engine written primarily in C++.

## Dependencies

#### Vulkan [[Windows](https://vulkan.lunarg.com/) / [Linux](https://vulkan-tutorial.com/Development_environment#page_Vulkan-Packages)]

Vulkan is a graphics API and serves as the main interface between our game engine and the hardware that will computing our graphics calculations. Vulkan is the rising industry standard in this regard, with OpenGL being its popular predecessor still widely used today. The Vulkan SDK will be required to run this instance of MAGE.

#### GLFW [[Windows](https://www.glfw.org/) / [Linux](https://vulkan-tutorial.com/Development_environment#page_GLFW-2)]

GLFW is a window creation API that is common amongst amatuer game engines both Vulkan and OpenGL alike. GLFW also assists with keyboard input monitoring for our camera.

#### glm [[Windows](https://github.com/g-truc/glm/releases) / [Linux](https://vulkan-tutorial.com/Development_environment#page_GLM-2)]

glm is a mathematics API that helps us handle formulas and computations with matrices in a relatively easy manner.

#### CMake [[Windows](https://cmake.org/download/) / [Linux](https://www.linuxfordevices.com/tutorials/install-cmake-on-linux)]

CMake is a cross-platform C++ tool that creates a Makefile for our project. A Makefile is used to compile our code and build an executable for us to run our project.

#### [glslc](https://github.com/google/shaderc/blob/main/downloads.md)

glslc is a shader compiling tool used to convert our vertex and fragment shaders (written in "glsl") into program-readable bytecode. glslc is a Google-based project, and alternatives like the Khronos glslangValidator can be used instead, but would require changes in the shader compilation scripts.

## Compiling and Building

#### Compiling Shaders

Shaders hold information of what our program wants to render, and as such it is required that we compile our shaders into program-readable bytecode before attempting to run the program. Use the script of your respective operating system to do so.

*Note: The Windows .bat file requires manual changes to point towards the directory of wherever glslc is located on your machine.*

#### Using CMake to Build

After cloning the repository and compiling our vertex and fragment shaders, you can create a "mage-game-engine" executable by generating a Makefile using CMake. This can be achieved by using the following commands:

` > cd /path/to/cloned/repository/ `

` > cmake .`

` > make `

After that, simply run the executable with:

` > ./mage-game-engine `

## TODO

I hope to achieve the following milestones before my Senior Project Day:

* ~~Create a window~~
* ~~R̶e̶n̶d̶e̶r̶ ̶a̶ ̶s̶i̶m̶p̶l̶e̶ ̶t̶r̶i̶a̶n̶g̶l̶e̶~~
* Render a cube
* Add camera and respond to user input
* Create a basic tutorial displaying the above features

I also hope to achieve this with as much cross-platform support as possible, but seeing as I'm developing on an NVIDIA-driven Linux machine, these particular systems will recieve precidence if time becomes an issue.

## Contact

You can reach out to me at my university email, [j_scharkey@u.pacific.edu.](mailto:j_scharkey@u.pacific.edu)
