#include "test-window.hpp"

namespace mage {

	void TestWindow::run(){
		while(!window.close_window()){
			glfwPollEvents();
		}
	}

}