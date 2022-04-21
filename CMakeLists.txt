cmake_minimum_required(VERSION 3.13)
project(mage-game-engine)

set(CMAKE_CXX_STANDARD 17)
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
set(GLFW_LIB glfw)
message(STATUS "Found GLFW")

file(GLOB_RECURSE SOURCES ./src/*.cpp)
add_executable(mage-game-engine ${SOURCES})

target_link_libraries(mage-game-engine glfw ${GLFW_LIBRARIES} Vulkan::Vulkan)
