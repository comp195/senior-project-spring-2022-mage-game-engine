# MAGE Game Engine

_MAGE_ (MAGE Amazing Game Engine) is the final senior project of Jordan Scharkey. It is a Vulkan-based, 3D game engine written primarily in C++.

## Dependencies and Compiling

#### Vulkan

Vulkan drivers for whichever operating system you are using will be required. You can find out more regarding this [here.](https://www.vulkan.org/tools#vulkan-gpu-resources)

#### Using CMake to Build

I've chosen to compile with CMake, as it offers better cross-platform compatability. After cloning the repository, you can create a "mage-game-engine" executable by generating a Makefile using CMake.

On Linux, this can be achieved by using the following commands:

` > cd /path/to/cloned/repository/ `

` > cmake `

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