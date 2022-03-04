#pragma once

#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace mage {

	class Window {
	private:
		int window_width;
		int window_height;
		std::string window_title;
		GLFWwindow *window;
		VkInstance instance;
		VkPhysicalDevice card = nullptr;
	public:
		Window(int w, int h, std::string title);
		~Window();
		void init_window();
		bool close_window();
		void init_vulkan_instance();
		void select_hardware();
		void logical_device();
		uint32_t queue_families(VkPhysicalDevice device);
	};

}