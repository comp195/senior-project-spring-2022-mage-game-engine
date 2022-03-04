#pragma once

#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <optional>

namespace mage {

	struct QueueIndices {
	    std::optional<uint32_t> graphics_family;
	    bool complete() {
	        return graphics_family.has_value();
	    }
	};

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
		bool suitable_device(VkPhysicalDevice);
		QueueIndices find_families(VkPhysicalDevice);
		//void logical_device();
	};

}