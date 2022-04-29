#include "transport.hpp"

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
  glm::mat4 transform{1.f};
  alignas(16) glm::vec3 color{};
};

TransportPass::TransportPass(DeviceHandling &device_pass, VkRenderPass render_pass) : device{device_pass} {
	std::cout << std::endl << "=== TRANSPORT PASS START ===" << std::endl;
  create_pipeline(render_pass);
  std::cout << "=== TRANSPORT PASS SUCCESSFUL ===" << std::endl;
}

void TransportPass::create_pipeline(VkRenderPass render_pass){
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
	if (vkCreatePipelineLayout(device.get_device(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
		std::cerr << "Failed to create pipeline layout" << std::endl;
	}

	std::cout << " - generating default pipeline configuration info..." << std::endl;
	PipelineInfo pipeline_config{};
  GraphicsPipeline::default_pipeline_info(pipeline_config);
  std::cout << " - grabbing swapchain render pass..." << std::endl;
	pipeline_config.render_pass = render_pass;
	pipeline_config.pipeline_layout = pipeline_layout;
	pipeline = std::make_unique<GraphicsPipeline>(device, pipeline_config);
  std::cout << " - pipeline creation successful...!?" << std::endl;
}

void TransportPass::render_game_objects(VkCommandBuffer command_buffer, std::vector<GameObject> &game_objects, const CameraHandling &camera){
	pipeline->bind(command_buffer);

	auto projection_view = camera.get_projection_matrix() * camera.get_view_matrix();

	for (auto& object : game_objects){
		object.transform.rotation.y = glm::mod(object.transform.rotation.y + 0.01f, glm::two_pi<float>());
		object.transform.rotation.x = glm::mod(object.transform.rotation.x + 0.01f, glm::two_pi<float>());

		push_constant_data push{};
		push.color = object.color;
		push.transform = projection_view * object.transform.mat4();

		vkCmdPushConstants(command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(push_constant_data), &push);
		object.model->bind(command_buffer);
		object.model->draw(command_buffer);

	}
}


TransportPass::~TransportPass() {
	vkDestroyPipelineLayout(device.get_device(), pipeline_layout, nullptr);
}
