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
		GLFWwindow* window;
		VkInstance instance;
		VkPhysicalDevice card = nullptr;
		VkDevice device;
		VkQueue graphics_queue;
		VkSurfaceKHR surface;
		VkQueue present_queue;
		VkSwapchainKHR swap_chain;
		VkPhysicalDeviceFeatures device_features{};
		std::vector<VkImage> swap_images;
		VkFormat swap_image_format;
		VkExtent2D swap_extent;
		std::vector<VkImageView> swap_image_views;
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
		void create_swap_chain();
		VkSurfaceFormatKHR choose_swap_format(const std::vector<VkSurfaceFormatKHR>&);
		VkPresentModeKHR choose_swap_mode(const std::vector<VkPresentModeKHR>&);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR&);
		void create_image_views();
		VkDevice get_device();
		VkExtent2D get_swap();
		void enable_nonFill();
		VkFormat get_format();
	};

}
