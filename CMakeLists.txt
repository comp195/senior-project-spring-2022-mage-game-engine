cmake_minimum_required(VERSION 3.13)
project(mage-game-engine)

set(CMAKE_CXX_STANDARD 17)
find_package(Vulkan REQUIRED)

add_executable(mage-game-engine main.cpp)
target_link_libraries(mage-game-engine glfw ${GLFW_LIBRARIES} Vulkan::Vulkan)
