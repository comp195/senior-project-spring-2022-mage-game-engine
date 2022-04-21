#pragma once

#include "window-resources/window.hpp"
#include "pipeline-resources/pipeline.hpp"
#include "pipeline-resources/device.hpp"
#include "pipeline-resources/swapchain.hpp"
#include "camera-resources/camera.hpp"
#include "object-resources/object.hpp"
#include <vector>
#include <memory>

namespace mage {

	class TestGame {
	private:	
		static const int WIDTH = 800;
		static const int HEIGHT = 600;
		static const int MAX_FRAMES_IN_FLIGHT = 2;
		std::string TITLE = "Mage Testing Window";
		std::vector<VkCommandBuffer> command_buffer;
  		size_t current_frame = 0;
  		VkPipelineLayout pipeline_layout;
  		std::vector<GameObject> game_objects;
	public:
		TestGame();
		~TestGame();
		std::unique_ptr<GraphicsPipeline> test_pipeline;
		Window test_game{WIDTH, HEIGHT, TITLE};
		DeviceHandling test_device{test_game};
		SwapChainHandling test_swap{test_device, test_game.get_extent()};
		CameraHandling test_camera{};
		void run();
		void create_command_buffer();
		void record_command_buffer(uint32_t image_index);
		void draw_frame();
		void sync_objects();
		void create_pipeline();
		VkResult acquire_next_image(uint32_t *image_index);
		VkResult submit_command_buffers(const VkCommandBuffer *buffers, uint32_t *image_index);
	};

}
