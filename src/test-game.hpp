#pragma once

#include "window-resources/window.hpp"
#include "pipeline-resources/pipeline.hpp"

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
		GraphicsPipeline pipeline{test_game};
		void run();
	};

}
