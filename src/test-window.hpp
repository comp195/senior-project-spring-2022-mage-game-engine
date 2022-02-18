#pragma once

#include "window-resources/window.hpp"

namespace mage {

	class TestWindow {
	public:
		static const int WIDTH = 800;
		static const int HEIGHT = 600;
		void run(){};
	private:
		Window test_window{WIDTH, HEIGHT, "MAGE Testing Window"};
	};

}