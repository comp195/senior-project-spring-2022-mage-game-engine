#include "pipeline.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#define GLFW_INCLUDE_VULKAN

using namespace mage;

GraphicsPipeline::GraphicsPipeline(VkDevice device_pass){
	// This is used to access the physical device that will be performing Vulkan-related processes
	device = device_pass;

	create_pipeline();
}

// Put together graphics pipeline
void GraphicsPipeline::create_pipeline(){
	auto vertex_bytecode = read_file("src/shaders/bytecode/vert.spv");
	auto fragment_bytecode = read_file("src/shaders/bytecode/frag.spv");
	vertex_module = create_module(vertex_bytecode);
	fragment_module = create_module(fragment_bytecode);

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

GraphicsPipeline::~GraphicsPipeline(){
	vkDestroyShaderModule(device, fragment_module, nullptr);
	vkDestroyShaderModule(device, vertex_module, nullptr);
}
