#include "test-game.hpp"
#include <iostream>

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

using namespace mage;

TestGame::TestGame() {
	create_pipeline();
	create_command_buffer();
}

// Run window until user wants to close it
void TestGame::run() {
	while(!test_game.close_window()){
		glfwPollEvents();
		draw_frame();
	}
	vkDeviceWaitIdle(test_device.get_device());
}


void TestGame::create_pipeline(){
	std::cout << "Attempting to create pipeline..." << std::endl;

	std::cout << " - creating info for pipeline layout..." << std::endl;
	VkPipelineLayoutCreateInfo pipeline_layout_info{};
 	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
 	pipeline_layout_info.setLayoutCount = 0;
  	pipeline_layout_info.pSetLayouts = nullptr;
  	pipeline_layout_info.pushConstantRangeCount = 0;
  	pipeline_layout_info.pPushConstantRanges = nullptr;
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
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = test_swap.get_render_pass();
    render_pass_info.framebuffer = test_swap.get_framebuffers(i);

    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = test_swap.get_swap_extent();

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clear_values[1].depthStencil = {1.0f, 0};
    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    test_pipeline->bind(command_buffer[i]);
    vkCmdDraw(command_buffer[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(command_buffer[i]);
    if (vkEndCommandBuffer(command_buffer[i]) != VK_SUCCESS) {
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
}
