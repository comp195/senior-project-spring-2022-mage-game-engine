#include "test-game.hpp"

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

struct push_constant_data {
  glm::vec4 transform{1.f};
  glm::vec3 color;
};

TestGame::TestGame() {
	create_pipeline();
	create_command_buffer();
}

// Run window until user wants to close it
void TestGame::run() {
  test_camera.set_view_direction(glm::vec3{0.f}, glm::vec3{0.5f, 0.f, 1.f});
	while(!test_game.close_window()){
		glfwPollEvents();
		draw_frame();
	}
	vkDeviceWaitIdle(test_device.get_device());
}


void TestGame::create_pipeline(){
	std::cout << "Attempting to create pipeline..." << std::endl;

  std::cout << " - filling data for push constants..." << std::endl;
  VkPushConstantRange push_constant_range{};
  push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  push_constant_range.offset = 0;
  push_constant_range.size = sizeof(push_constant_data);

	std::cout << " - creating info for pipeline layout..." << std::endl;
	VkPipelineLayoutCreateInfo pipeline_layout_info{};
 	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
 	pipeline_layout_info.setLayoutCount = 0;
  	pipeline_layout_info.pSetLayouts = nullptr;
  	pipeline_layout_info.pushConstantRangeCount = 1;
  	pipeline_layout_info.pPushConstantRanges = &push_constant_range;
  	std::cout << " - creating pipeline layout..." << std::endl;
  	if (vkCreatePipelineLayout(test_device.get_device(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
  		std::cerr << "Failed to create pipeline layout" << std::endl;
  	}

  	std::cout << " - generating default pipeline configuration info..." << std::endl;
 	auto pipeline_config = GraphicsPipeline::default_pipeline_info(test_swap.get_swap_extent());
  	pipeline_config.render_pass = test_swap.get_render_pass();
  	pipeline_config.pipeline_layout = pipeline_layout;
  	test_pipeline = std::make_unique<GraphicsPipeline>(test_device, pipeline_config);
}


void TestGame::create_command_buffer(){
  command_buffer.resize(test_swap.get_image_count());

  VkCommandBufferAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandPool = test_device.get_command_pool();
  allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffer.size());

  if (vkAllocateCommandBuffers(test_device.get_device(), &allocate_info, command_buffer.data()) !=
      VK_SUCCESS) {
  }

  for (int i = 0; i < command_buffer.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer[i], &beginInfo) != VK_SUCCESS) {
    	std::cerr << "Failed to begin creating command_buffer" << std::endl;
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = test_swap.get_render_pass();
    render_pass_info.framebuffer = test_swap.get_framebuffers(i);

    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = test_swap.get_swap_extent();

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clear_values[1].depthStencil = {1.0f, 0};
    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    auto projection_view = test_camera.get_projection_matrix() * test_camera.get_view_matrix();

    for (auto &object : game_objects) {
      object.transform.rotation.y = glm::mod(object.transform.rotation.y + 0.01f, glm::two_pi<float>());
      object.transform.rotation.x = glm::mod(object.transform.rotation.x + 0.005f, glm::two_pi<float>());

      push_constant_data push{};
      push.color = object.color;
      push.transform = projection_view * object.transform.mat4();

      vkCmdPushConstants(command_buffer[i], pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                         0, sizeof(push_constant_data), &push);

      test_pipeline->bind(command_buffer[i]);
      vkCmdDraw(command_buffer[i], 3, 1, 0, 0);
    }

    vkCmdEndRenderPass(command_buffer[i]);
    if (vkEndCommandBuffer(command_buffer[i]) != VK_SUCCESS) {
    	std::cerr << "Failed to end command buffer" << std::endl;
    }
  }
	
}

void TestGame::draw_frame() {
  uint32_t image_index;

  std::cout << "Attempting to acquire next image..." << std::endl;
  auto result = test_swap.acquire_next_image(&image_index);
  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
  	std::cerr << "Failed to acquire next image" << std::endl;
  }

  result = test_swap.submit_command_buffers(&command_buffer[image_index], &image_index);
  if (result != VK_SUCCESS) {
  	std::cerr << "Failed to submit command buffer" << std::endl;
  }
}


TestGame::~TestGame() {
	vkDestroyPipelineLayout(test_device.get_device(), pipeline_layout, nullptr);
}
