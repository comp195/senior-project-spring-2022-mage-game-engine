#include "window.hpp"

namespace mage {

	// Window constructor
	Window::Window(int w, int h, std::string title){
		window_width = w;
		window_height = h;
		window_title = title;
		init_window();
	}

	// Creation of window via GLFW
	void Window::init_window(){
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);
	}

	// Reflects whether user is attempting to currently close window
	bool Window::close_window(){
		return glfwWindowShouldClose(window);
	}

	// Free resources after closed window
	Window::~Window(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}


}