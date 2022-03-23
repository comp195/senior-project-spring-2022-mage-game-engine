#pragma once

#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace mage {

	class Window {
	private:
		int window_width;
		int window_height;
		std::string window_title;
		GLFWwindow* window;
	public:
		Window(int w, int h, std::string title);
		~Window();
		void init_window();
		bool close_window();
		void create_surface(VkInstance instance, VkSurfaceKHR surface);
	};

}
