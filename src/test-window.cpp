#include "test-window.hpp"

using namespace mage;

void TestWindow::run(){
	while(!test_window.close_window()){
		glfwPollEvents();
	}
}