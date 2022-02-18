#pragma once

#include "window-resources/window.hpp"

namespace mage {

	class TestWindow {
	public:
		Window test_window{WIDTH, HEIGHT, "MAGE Testing Window"};
		static const int WIDTH = 800;
		static const int HEIGHT = 600;
		void run();
	};

}