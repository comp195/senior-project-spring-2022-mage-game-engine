#include "pipeline.hpp"
#include <string>
#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#define GLFW_INCLUDE_VULKAN

using namespace mage;



// Constructor
GraphicsPipeline::GraphicsPipeline(DeviceHandling& device_pass){	
	std::cout << "Transferring VkDevice to Vulkan pipeline file..." << std::endl;
	device = device_pass.get_device();
	
	std::cout << std::endl << "Beginning pipeline construction..." << std::endl;
	create_pipeline(device_pass);
}

// Put together graphics pipeline
void GraphicsPipeline::create_pipeline(DeviceHandling& device_pass){

	std::cout << "Filling default pipeline information... " << std::endl;
	PipelineInfo config_info = default_pipeline_info(config_info, device_pass);
	std::cout << "Pipeline information complete" << std::endl;

	std::cout << "Reading shader bytecode..." << std::endl;
	// Read bytecode from shaders and create Vulkan modules for them
	auto vertex_bytecode = read_file("src/shaders/vert.spv");
	auto fragment_bytecode = read_file("src/shaders/frag.spv");

	std::cout << "Creating shader modules..." << std::endl;
	vertex_module = create_module(vertex_bytecode);
	fragment_module = create_module(fragment_bytecode);

	// Fillout Vulkan object info regarding shader modules
	std::cout << "Reading in shader information structures..." << std::endl;
	VkPipelineShaderStageCreateInfo vertex_info{};
	vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertex_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertex_info.module = vertex_module;
	vertex_info.pName = "main";

	VkPipelineShaderStageCreateInfo fragment_info{};
	fragment_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragment_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	fragment_info.module = fragment_module;
	fragment_info.pName = "main";

	VkPipelineShaderStageCreateInfo shader_info[] = {vertex_info, fragment_info};

	std::cout << "Vertex input info structure..." << std::endl;
	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexAttributeDescriptionCount = 0;
	vertex_input_info.vertexBindingDescriptionCount = 0;

	std::cout << "Constructing pipeline layout..." << std::endl;
	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS){
		std::cerr << "Failed to create pipeline layout";
		exit(EXIT_FAILURE);
	}

	std::cout << "Constructing render pass..." << std::endl;
	VkAttachmentDescription color_attachment{};
	color_attachment.format = device_pass.get_swap_format();
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_reference{};
	color_reference.attachment = 0;
	color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_reference;

	VkRenderPassCreateInfo render_info{};
	render_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_info.attachmentCount = 1;
	render_info.pAttachments = &color_attachment;
	render_info.subpassCount = 1;
	render_info.pSubpasses = &subpass;

	if (vkCreateRenderPass(device, &render_info, nullptr, &render_pass) != VK_SUCCESS){
		std::cerr << "Failed to create render pass";
		exit(EXIT_FAILURE);
	}


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
	//pipe_info.pDynamicState = &config_info.dynamic_state;

	pipe_info.layout = pipeline_layout;
	pipe_info.renderPass = render_pass;
	pipe_info.subpass = 0;
	pipe_info.basePipelineHandle = VK_NULL_HANDLE;
	pipe_info.basePipelineIndex = -1;

	// Moment of truth
	std::cout << "Final creation of pipeline..." << std::endl;
	 if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe_info, nullptr, &graphics_pipeline) != VK_SUCCESS){
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
	if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
		std::cerr << "Failed to create shader module";
		exit(EXIT_FAILURE);
	}

	return shader_module;
}



PipelineInfo GraphicsPipeline::default_pipeline_info(PipelineInfo& config_info, DeviceHandling& device_pass){
	std::cout << " - input_assembly_info..." << std::endl;
	config_info.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	config_info.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

	std::cout << " - viewport_info..." << std::endl;

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) device_pass.get_swap_extent().width;
	viewport.height = (float) device_pass.get_swap_extent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = {0, 0};
	scissor.extent = device_pass.get_swap_extent();

	config_info.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	config_info.viewport_info.viewportCount = 1;
	config_info.viewport_info.pViewports = &viewport;
	config_info.viewport_info.scissorCount = 1;
	config_info.viewport_info.pScissors = &scissor;

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

GraphicsPipeline::~GraphicsPipeline(){
	vkDestroyPipeline(device, graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
	vkDestroyShaderModule(device, fragment_module, nullptr);
	vkDestroyShaderModule(device, vertex_module, nullptr);
	vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
	vkDestroyRenderPass(device, render_pass, nullptr);
}
