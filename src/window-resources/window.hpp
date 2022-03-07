#pragma once

#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace mage {

	struct QueueIndices {
	    std::optional<uint32_t> graphics_family;
	    std::optional<uint32_t> present_family;
	    bool complete() {
	        return graphics_family.has_value() && present_family.has_value();
	    }
	};

	struct SwapChainSupport {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	class Window {
	private:
		int window_width;
		int window_height;
		std::string window_title;
		GLFWwindow *window;
		VkInstance instance;
		VkPhysicalDevice card = nullptr;
		VkDevice device;
		VkQueue graphics_queue;
		VkSurfaceKHR surface;
		VkQueue present_queue;
	public:
		Window(int w, int h, std::string title);
		~Window();
		void init_window();
		bool close_window();
		void init_vulkan_instance();
		void select_hardware();
		bool suitable_device(VkPhysicalDevice);
		QueueIndices find_families(VkPhysicalDevice);
		void logical_device();
		void create_surface();
		bool check_extension_support(VkPhysicalDevice);	
		SwapChainSupport query_support(VkPhysicalDevice);
	};

}
