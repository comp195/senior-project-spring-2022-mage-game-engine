#pragma once

#include "window-resources/window.hpp"
#include "pipeline-resources/device.hpp"
#include "pipeline-resources/artist.hpp"
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
  		size_t current_frame = 0;
  		std::vector<GameObject> game_objects;
	public:
		TestGame();
		~TestGame();
		std::unique_ptr<GraphicsPipeline> test_pipeline;
		std::unique_ptr<SwapChainHandling> test_swap;
		Window test_game{WIDTH, HEIGHT, TITLE};
		DeviceHandling test_device{test_game};
		DrawHandling test_artist{test_game, test_device};
		CameraHandling test_camera{};
		void run();
		void sync_objects();
		void load_game_objects();
		VkResult acquire_next_image(uint32_t *image_index);
		VkResult submit_command_buffers(const VkCommandBuffer *buffers, uint32_t *image_index);
	};

}
