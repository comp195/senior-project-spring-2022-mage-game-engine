#include "pipeline.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#define GLFW_INCLUDE_VULKAN

using namespace mage;

// Constructor
GraphicsPipeline::GraphicsPipeline(Window& window_pass){
	// This is used to access the physical device that will be performing Vulkan-related processes
	device = window_pass.get_device();

	create_viewport(window_pass.get_swap());
	create_pipeline();
}

// Put together graphics pipeline
void GraphicsPipeline::create_pipeline(){
	// Read bytecode from shaders and create Vulkan modules for them
	auto vertex_bytecode = read_file("src/shaders/bytecode/vert.spv");
	auto fragment_bytecode = read_file("src/shaders/bytecode/frag.spv");
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
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // this can be changed with mage::change_rasterizer_mode(VkStructureType)
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
}

// Used to change rasterizer polygon mode (if you'd like)
void GraphicsPipeline::change_rasterizer_mode(Window window_pass, VkPolygonMode poly){
	// poly can be any of the following types:
	// VK_POLYGON_MODE_FILL (default)
	// VK_POLYGON_MODE_LINE
	// VK_POLYGON_MODE_POINT
	
	if (poly != VK_POLYGON_MODE_FILL){
		window_pass.enable_nonFill();
	}

	rasterizer.polygonMode = poly;

}


GraphicsPipeline::~GraphicsPipeline(){
	vkDestroyShaderModule(device, fragment_module, nullptr);
	vkDestroyShaderModule(device, vertex_module, nullptr);
}
