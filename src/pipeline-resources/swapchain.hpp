#pragma once

#include "../window-resources/window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include <vector>
#include <memory>

namespace mage {

	class SwapChainHandling {
	private:	
		static const int MAX_FRAMES = 2;
		size_t current_frame = 0;
		DeviceHandling &device;
		VkExtent2D window_extent;
		VkFormat swap_image_format;
		VkFormat swap_depth_format;
		VkExtent2D swap_extent;
		VkSwapchainKHR swap_chain;
  		std::vector<VkFramebuffer> swap_chain_framebuffers;
  		VkRenderPass render_pass;
  		std::vector<VkImage> depth_images;
  		std::vector<VkDeviceMemory> depth_images_memories;
  		std::vector<VkImageView> depth_images_views;
  		std::vector<VkImage> swap_images;
  		std::vector<VkImageView> swap_image_views;
  		std::vector<VkSemaphore> image_available_semaphores;
  		std::vector<VkSemaphore> render_available_semaphores;
  		std::vector<VkFence> in_flight_fences;
  		std::vector<VkFence> images_in_flight;
  		std::shared_ptr<SwapChainHandling> old_swapchain;
	public:
		SwapChainHandling(DeviceHandling &device_pass, VkExtent2D window_extent);
		SwapChainHandling(DeviceHandling &device_pass, VkExtent2D window_extent, std::shared_ptr<SwapChainHandling> previous);
		~SwapChainHandling();
		void create_swap_chain();
		void create_image_views();
		void create_render_pass();
		void create_depth_resources();
		void create_framebuffers();
		void create_sync_objects();
		VkSurfaceFormatKHR choose_swap_format(const std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR choose_swap_mode(const std::vector<VkPresentModeKHR>& modes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkFormat find_depth_format();
		void create_image(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &image_memory);
		uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
		VkResult acquire_next_image(uint32_t *image_index);
		VkResult submit_command_buffers(const VkCommandBuffer *buffers, uint32_t *image_index);

		int get_max_frames(){return MAX_FRAMES;}
		VkExtent2D get_swap_extent(){return swap_extent;}
		VkRenderPass get_render_pass(){return render_pass;}
		VkFramebuffer get_framebuffers(int index) {return swap_chain_framebuffers[index];}
		size_t get_image_count(){return swap_images.size();}
		bool compare_swap_formats(const SwapChainHandling &swapchain) const {
    		return swapchain.swap_depth_format == swap_depth_format && swapchain.swap_image_format == swap_image_format;
  		}
	};

}
