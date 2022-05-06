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
		static const int WIDTH = 1520;
		static const int HEIGHT = 1000;
		std::string TITLE = "Mage Testing Window";
  		std::vector<GameObject> game_objects;
	public:
		TestGame();
		~TestGame();
		Window test_game{WIDTH, HEIGHT, TITLE};
		DeviceHandling test_device{test_game};
		DrawHandling test_artist{test_game, test_device};
		CameraHandling test_camera{};
		void run();
		void load_game_objects();
	};

}
