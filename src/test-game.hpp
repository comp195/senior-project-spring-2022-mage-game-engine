#pragma once

#include "window-resources/window.hpp"
#include "pipeline-resources/pipeline.hpp"
#include "pipeline-resources/device.hpp"

namespace mage {

	class TestGame {
	private:	
		static const int WIDTH = 800;
		static const int HEIGHT = 600;
		std::string TITLE = "Mage Testing Window";
		VkCommandBuffer command_buffer;
		VkSemaphore image_available_semaphores;
  		VkSemaphore render_available_semaphores;
  		VkFence in_flight_fences;
	public:
		TestGame();
		~TestGame();
		Window test_game{WIDTH, HEIGHT, TITLE};
		DeviceHandling test_device{test_game};
		GraphicsPipeline test_pipeline{test_device};
		void run();
		void create_command_buffer();
		void record_command_buffer(uint32_t image_index);
		void draw_frame();
		void sync_objects();
	};

}
