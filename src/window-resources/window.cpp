#include "window.hpp"
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <cstdlib>
#include <vector>
#include <unordered_set>
#include <set>
#include <limits>
#include <algorithm>

using namespace mage;


const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

// Window constructor
Window::Window(int w, int h, std::string title){
	window_width = w;
	window_height = h;
	window_title = title;
	init_window();
}


// Creation of window via GLFW
void Window::init_window(){
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);
}

// Attempts to create surface to connect Vulkan to window
void Window::create_surface(VkInstance instance, VkSurfaceKHR *surface) {
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
        exit(EXIT_FAILURE);
    }
}

// Reflects whether user is attempting to currently close window
bool Window::close_window(){
	return glfwWindowShouldClose(window);
}


// Free resources after closed window
Window::~Window(){
	glfwDestroyWindow(window);
	glfwTerminate();
}
