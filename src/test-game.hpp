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
	public:
		TestGame();
		~TestGame();
		Window test_game{WIDTH, HEIGHT, TITLE};
		DeviceHandling test_device{test_game};
		GraphicsPipeline test_pipeline{test_device};
		void run();
	};

}
