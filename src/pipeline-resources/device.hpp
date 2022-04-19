#pragma once

#include "../window-resources/window.hpp"
#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace mage {

	struct QueueIndices {
	    uint32_t graphics_family;
	    uint32_t present_family;
	    std::optional<bool> graphics_family_has_value = false;
	    std::optional<bool> present_family_has_value = false;
	    bool complete() {
	        return graphics_family_has_value && present_family_has_value;
	    }
	};

	struct SwapChainSupport {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	class DeviceHandling {
	private:
		Window& window;
		VkInstance instance;
		VkPhysicalDevice card{};
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
		VkCommandPool command_pool;
		VkCommandBuffer command_buffer;
	public:
		DeviceHandling(Window &window_pass);
		~DeviceHandling();
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
		VkFormat find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		void create_image_views();

		VkExtent2D get_swap_extent(){
			return swap_extent;
		}

		VkFormat get_swap_format(){
			return swap_image_format;
		}

		VkCommandPool get_command_pool(){
			return command_pool;
		}
		VkSwapchainKHR get_swap_chain(){
			return swap_chain;
		}
		VkQueue get_graphics_queue(){
			return graphics_queue;
		}
		VkQueue get_present_queue(){
			return present_queue;
		}
		size_t get_swap_image_size(){
			return swap_image_views.size();
		}
		SwapChainSupport get_swap_chain_support(){
			return query_support(card);
		}
		QueueIndices get_queue_families(){
			return find_families(card);
		}
		VkSurfaceKHR get_surface(){
			return surface;
		}
		VkPhysicalDevice get_card(){
			return card;
		}

		std::vector<VkImageView> get_swap_view();
		void check_glfw_extensions();
		std::vector<const char*> get_required_extensions();
		void create_command_pool();
		VkDevice get_device();

	};

}
