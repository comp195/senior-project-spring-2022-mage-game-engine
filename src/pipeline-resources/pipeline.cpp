#include "pipeline.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#define GLFW_INCLUDE_VULKAN

using namespace mage;

// Constructor
GraphicsPipeline::GraphicsPipeline(DeviceHandling& device_pass){	
	create_viewport(device_pass.get_swap_extent());
	//create_pipeline(device_pass.get_swap_format());
}

// Put together graphics pipeline
void GraphicsPipeline::create_pipeline(VkFormat format){
	// Read bytecode from shaders and create Vulkan modules for them
	auto vertex_bytecode = read_file("src/shaders/vert.spv");
	auto fragment_bytecode = read_file("src/shaders/frag.spv");
	vertex_module = create_module(vertex_bytecode);
	fragment_module = create_module(fragment_bytecode);

	// Fillout Vulkan object info regarding shader modules
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

	// Hard coding some vertex info stuff for now, will be fixed with buffers later
	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 0;
	vertex_input_info.vertexAttributeDescriptionCount = 0;
	VkPipelineInputAssemblyStateCreateInfo assembly_info{};
	assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assembly_info.primitiveRestartEnable = VK_FALSE;

	// Our viewpoint is set to view the entire image- lets create more object info about it
	VkPipelineViewportStateCreateInfo viewport_info{};
	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.viewportCount = 1;
	viewport_info.pViewports = &viewport;
	viewport_info.scissorCount = 1;
	viewport_info.pScissors = &scissor;

	// Rasterizing options are unique, so I put them in its own function
	config_rasterizer();

	// Multisampling info
	VkPipelineMultisampleStateCreateInfo multisample_info{};
	multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_info.sampleShadingEnable = VK_FALSE;

	// Framebuffer will be expanded upon later, so I put it in its own function
	config_blending();

	config_render_pass(format);

	//// Some final pipeline struct declarations
	VkPipelineLayoutCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_info.setLayoutCount = 0;
	pipeline_info.pSetLayouts = nullptr;
	pipeline_info.pushConstantRangeCount = 0;
	pipeline_info.pPushConstantRanges = nullptr;

	// Now we can attempt to bring the pipeline layout together
	if (vkCreatePipelineLayout(device, &pipeline_info, nullptr, &pipeline_layout) != VK_SUCCESS){
		std::cerr << "Failed to create pipeline layout";
		exit(EXIT_FAILURE);
	}

	// Now to put it all together...
	VkGraphicsPipelineCreateInfo pipe_info{};
	pipe_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipe_info.stageCount = 2;
	pipe_info.pStages = shader_info;
	pipe_info.pVertexInputState = &vertex_input_info;
	pipe_info.pInputAssemblyState = &assembly_info;
	pipe_info.pViewportState = &viewport_info;
	pipe_info.pRasterizationState = &rasterizer;
	pipe_info.pMultisampleState = &multisample_info;
	pipe_info.pColorBlendState = &color_info;
	pipe_info.layout = pipeline_layout;
	pipe_info.renderPass = render_pass;
	pipe_info.subpass = 0;
	pipe_info.basePipelineHandle = VK_NULL_HANDLE;
	pipe_info.basePipelineIndex = -1;
	std::cout << "here3";

	// Moment of truth
	 if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe_info, nullptr, &graphics_pipeline) != VK_SUCCESS){
	 	std::cerr << "Failed to create graphics pipeline";
	 	exit(EXIT_FAILURE);
	 }

	std::cout << "here4";
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

// Open viewport for display
void GraphicsPipeline::create_viewport(VkExtent2D swap_extent){
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swap_extent.width;
	viewport.height = (float) swap_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;	

	scissor.offset = {0, 0};
	scissor.extent = swap_extent;
}

// Initialize rasterizer settings
void GraphicsPipeline::config_rasterizer(){
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
}


// Color blending and initial framebuffer settings
void GraphicsPipeline::config_blending(){
	VkPipelineColorBlendAttachmentState blending_info{};
	blending_info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blending_info.blendEnable = VK_FALSE;
	blending_info.blendEnable = VK_TRUE;
	blending_info.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blending_info.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blending_info.colorBlendOp = VK_BLEND_OP_ADD;
	blending_info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	blending_info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	blending_info.alphaBlendOp = VK_BLEND_OP_ADD;

	color_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_info.logicOpEnable = VK_TRUE;
	color_info.attachmentCount = 1;
	color_info.pAttachments = &blending_info;
}

// Attachments for better describing the framebuffer
void GraphicsPipeline::config_render_pass(VkFormat format){
	VkAttachmentDescription color_attachment{};
	color_attachment.format = format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachment_reference{};
	attachment_reference.attachment = 0;
	attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachment_reference;

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

}


GraphicsPipeline::~GraphicsPipeline(){
	vkDestroyPipeline(device, graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
	vkDestroyShaderModule(device, fragment_module, nullptr);
	vkDestroyShaderModule(device, vertex_module, nullptr);
	vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
	vkDestroyRenderPass(device, render_pass, nullptr);
}
