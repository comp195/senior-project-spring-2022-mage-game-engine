#include "test-game.hpp"
#include "object-resources/transport.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <chrono>

using namespace mage;

TestGame::TestGame() {
  std::cout << std::endl << "=== LOADING GAME OBJECTS ===" << std::endl; 
	load_game_objects();
  std::cout << "=== LOADING GAME SUCCESSFUL ===" << std::endl;
}

// Run window until user wants to close it
void TestGame::run() {
  std::cout << "Attempting to begin running game..." << std::endl;

  std::cout << " - handling pipeline creation to transport..." << std::endl;
  TransportPass test_transport{test_device, test_artist.get_swapchain_render_pass()};
  std::cout << " - initializing camera..." << std::endl;
  test_camera.set_view_direction(glm::vec3{0.f}, glm::vec3{0.5f, 0.f, 1.f});

	while(!test_game.close_window()){
		glfwPollEvents();
    float aspect_ratio = test_artist.get_aspect_ratio();
    test_camera.set_perspective_projection(glm::radians(50.f), aspect_ratio, 0.1f, 10.f);
    if (auto command_buffer = test_artist.draw_start()){
      test_artist.swapchain_render_start(command_buffer);
      test_transport.render_game_objects(command_buffer, game_objects, test_camera);
      test_artist.swapchain_render_end(command_buffer);
      test_artist.draw_end();
    }
	}
	vkDeviceWaitIdle(test_device.get_device());
}

// The specific values for this test cube are provided by https://github.com/blurrypiano
std::unique_ptr<GameModel> create_cube_model(DeviceHandling &device, glm::vec3 offset) {
  std::vector<GameModel::Vertex> vertices{

      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

  };
  for (auto& v : vertices) {
    v.position += offset;
  }
  return std::make_unique<GameModel>(device, vertices);
}

void TestGame::load_game_objects() {
  std::cout << "Attempting to create cube..." << std::endl;
  std::shared_ptr<GameModel> model = create_cube_model(test_device, {.0f, .0f, .0f});
  auto cube = GameObject::create_game_object();
  cube.model = model;
  cube.transform.translation = {.0f, .0f, 2.5f};
  cube.transform.scale = {.5f, .5f, .5f};
  game_objects.push_back(std::move(cube));
  std::cout << " - cube creation successful!" << std::endl;
}


TestGame::~TestGame() {
	
}
