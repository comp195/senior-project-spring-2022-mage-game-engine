#pragma once

#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace mage {

	class Window {
	private:
		int window_width;
		int window_height;
		std::string window_title;
		GLFWwindow *window;

		void init_window();
		bool close_window();
	public:
		Window(int w, int h, std::string title);
		~Window();
	};

}