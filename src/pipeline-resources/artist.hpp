#pragma once

#include "../window-resources/window.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include <vector>
#include <memory>

namespace mage {

	class DrawHandling {
	private:	
		std::vector<VkCommandBuffer> command_buffer;
		int current_frame{0};
		uint32_t current_image;
		bool frame_started = false;
	public:
		DrawHandling(Window &window_pass, DeviceHandling &device_pass);
		~DrawHandling();
		Window &window;
		DeviceHandling &device;
		std::unique_ptr<SwapChainHandling> swapchain;
		void create_command_buffer();
		void free_command_buffer();
		VkCommandBuffer draw_start();
		void draw_end();
		void swapchain_render_start(VkCommandBuffer current_command_buffer);
		void swapchain_render_end(VkCommandBuffer current_command_buffer);
		void sync_objects();
		void create_pipeline();
		void create_swapchain();

		bool is_frame_in_progres() const {return frame_started;}
		VkCommandBuffer get_current_command_buffer() const {return command_buffer[current_frame];}
		VkRenderPass get_swapchain_render_pass() const {return swapchain->get_render_pass();}
		float get_aspect_ratio() const { return (swapchain->get_swap_extent().width / swapchain->get_swap_extent().height);}
	};

}
