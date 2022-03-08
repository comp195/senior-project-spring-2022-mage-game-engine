#pragma once

#include "window-resources/window.hpp"

namespace mage {

	class TestGame {
	private:	
		static const int WIDTH = 800;
		static const int HEIGHT = 600;
		std::string TITLE = "Mage Testing Window";
		Window test_game{WIDTH, HEIGHT, TITLE};
	public:
		TestGame();
		~TestGame();
		void run();
	};

}
