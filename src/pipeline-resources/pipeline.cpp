#include "pipeline.hpp"
#include <string>
#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#include <array>
#define GLFW_INCLUDE_VULKAN

using namespace mage;



// Constructor
GraphicsPipeline::GraphicsPipeline(DeviceHandling& device_pass, PipelineInfo& config_info) : device{device_pass} {	
	std::cout << std::endl << "Beginning pipeline construction..." << std::endl;
	create_pipeline(config_info);
}

// Put together graphics pipeline
void GraphicsPipeline::create_pipeline(const PipelineInfo config_info){

	std::cout << "Reading shader bytecode..." << std::endl;
	// Read bytecode from shaders and create Vulkan modules for them
	auto vertex_bytecode = read_file("src/shaders/vert.spv");
	auto fragment_bytecode = read_file("src/shaders/frag.spv");

	std::cout << "Creating shader modules..." << std::endl;
	vertex_module = create_module(vertex_bytecode);
	fragment_module = create_module(fragment_bytecode);

	// Fillout Vulkan object info regarding shader modules
	std::cout << "Reading in shader information structures..." << std::endl;
  	VkPipelineShaderStageCreateInfo shader_info[2];
  	shader_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  	shader_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  	shader_info[0].module = vertex_module;
  	shader_info[0].pName = "main";
  	shader_info[0].flags = 0;
  	shader_info[0].pNext = nullptr;
  	shader_info[0].pSpecializationInfo = nullptr;
  	shader_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  	shader_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  	shader_info[1].module = fragment_module;
  	shader_info[1].pName = "main";
  	shader_info[1].flags = 0;
  	shader_info[1].pNext = nullptr;
  	shader_info[1].pSpecializationInfo = nullptr;

	std::cout << "Vertex input info structure..." << std::endl;
	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  	vertex_input_info.vertexAttributeDescriptionCount = 0;
  	vertex_input_info.vertexBindingDescriptionCount = 0;
  	vertex_input_info.pVertexAttributeDescriptions = nullptr;
  	vertex_input_info.pVertexBindingDescriptions = nullptr;

  	std::cout << "Viewport info..." << std::endl;
  	VkPipelineViewportStateCreateInfo viewportInfo{};
  	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  	viewportInfo.viewportCount = 1;
  	viewportInfo.pViewports = &config_info.viewport;
  	viewportInfo.scissorCount = 1;
  	viewportInfo.pScissors = &config_info.scissor;

	// Now to put it all together...
	std::cout << "Attempting to bring together pipeline information..." << std::endl;
	VkGraphicsPipelineCreateInfo pipe_info{};
	pipe_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipe_info.stageCount = 2;
	pipe_info.pStages = shader_info;
	pipe_info.pVertexInputState = &vertex_input_info;
	pipe_info.pInputAssemblyState = &config_info.input_assembly_info;
	pipe_info.pViewportState = &config_info.viewport_info;
	pipe_info.pRasterizationState = &config_info.rasterization_info;
	pipe_info.pMultisampleState = &config_info.multisample_info;
	pipe_info.pColorBlendState = &config_info.color_blend_info;
	pipe_info.pDepthStencilState = &config_info.depth_stencil_info;
	pipe_info.pDynamicState = nullptr;

	pipe_info.layout = config_info.pipeline_layout;
	pipe_info.renderPass = config_info.render_pass;
	pipe_info.subpass = config_info.subpass;
	pipe_info.basePipelineHandle = VK_NULL_HANDLE;
	pipe_info.basePipelineIndex = -1;

	// Moment of truth
	std::cout << "Final creation of pipeline..." << std::endl;
	 if (vkCreateGraphicsPipelines(device.get_device(), VK_NULL_HANDLE, 1, &pipe_info, nullptr, &graphics_pipeline) != VK_SUCCESS){
	 	std::cerr << "Failed to create graphics pipeline";
	 	exit(EXIT_FAILURE);
	 }

	 std::cout << "Pipeline construction complete!" << std::endl;

}

// Used to read data from shader files before creating their shader modules
std::vector<char> GraphicsPipeline::read_file(const std::string& file_name){
	// Attempt to open file
	std::ifstream file(file_name, std::ios::ate | std::ios::binary);
	if(!file.is_open()){
		std::cerr << "Failed to open file";
		exit(EXIT_FAILURE);
	}

	// Read data from file
	size_t file_size = (size_t) file.tellg();
	std::vector<char> buffer(file_size);
	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();

	return buffer;

}

// Generalized module creation
VkShaderModule GraphicsPipeline::create_module(const std::vector<char>& data){

	// Obstantiate object info regarding module
	VkShaderModuleCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = data.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(data.data());

	VkShaderModule shader_module;
	if (vkCreateShaderModule(device.get_device(), &create_info, nullptr, &shader_module) != VK_SUCCESS) {
		std::cerr << "Failed to create shader module";
		exit(EXIT_FAILURE);
	}

	return shader_module;
}



PipelineInfo GraphicsPipeline::default_pipeline_info(VkExtent2D swap_extent){
	PipelineInfo config_info;

	std::cout << " - input_assembly_info..." << std::endl;
	config_info.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	config_info.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

	std::cout << " - viewport_info..." << std::endl;

	config_info.viewport.x = 0.0f;
	config_info.viewport.y = 0.0f;
	config_info.viewport.width = (float) swap_extent.width;
	config_info.viewport.height = (float) swap_extent.height;
	config_info.viewport.minDepth = 0.0f;
	config_info.viewport.maxDepth = 1.0f;

	config_info.scissor.offset = {0, 0};
	config_info.scissor.extent = swap_extent;

	config_info.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	config_info.viewport_info.viewportCount = 1;
	config_info.viewport_info.pViewports = &config_info.viewport;
	config_info.viewport_info.scissorCount = 1;
	config_info.viewport_info.pScissors = &config_info.scissor;

	std::cout << " - rasterization_info..." << std::endl;
	config_info.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	config_info.rasterization_info.depthClampEnable = VK_FALSE;
	config_info.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
	config_info.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
	config_info.rasterization_info.lineWidth = 1.0f;
	config_info.rasterization_info.cullMode = VK_CULL_MODE_NONE;
	config_info.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	config_info.rasterization_info.depthBiasEnable = VK_FALSE;
	
	std::cout << " - multisample_info..." << std::endl;
	config_info.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	config_info.multisample_info.sampleShadingEnable = VK_FALSE;
	config_info.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	std::cout << " - color_blend_attachment..." << std::endl;
	config_info.color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	config_info.color_blend_attachment.blendEnable = VK_FALSE;
	
	std::cout << " - color_blend_info..." << std::endl;
	config_info.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	config_info.color_blend_info.logicOpEnable = VK_FALSE;
	config_info.color_blend_info.attachmentCount = 1;
	config_info.color_blend_info.pAttachments = &config_info.color_blend_attachment;

	std::cout << " - depth_stencil_info..." << std::endl;
	config_info.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	config_info.depth_stencil_info.depthTestEnable = VK_TRUE;
	config_info.depth_stencil_info.depthWriteEnable = VK_TRUE;
	config_info.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	config_info.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	config_info.depth_stencil_info.stencilTestEnable = VK_FALSE;

	//std::cout << " - dynamic_state information..." << std::endl;
	//config_info.dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	//config_info.dynamic_state.dynamicStateCount = static_cast<uint32_t>(config_info.dynamic_states.size());
	//config_info.dynamic_state.pDynamicStates = config_info.dynamic_states.data();

	std::cout << " - returning config_info..." << std::endl;
	
	return config_info;
}

void GraphicsPipeline::bind(VkCommandBuffer command_buffer) {
  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
}


GraphicsPipeline::~GraphicsPipeline(){
	vkDestroyPipeline(device.get_device(), graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(device.get_device(), pipeline_layout, nullptr);
	vkDestroyShaderModule(device.get_device(), fragment_module, nullptr);
	vkDestroyShaderModule(device.get_device(), vertex_module, nullptr);
	vkDestroyPipelineLayout(device.get_device(), pipeline_layout, nullptr);
	vkDestroyRenderPass(device.get_device(), render_pass, nullptr);
	for (auto framebuffer : swap_chain_framebuffers) { 
		vkDestroyFramebuffer(device.get_device(), framebuffer, nullptr);
	}
}
