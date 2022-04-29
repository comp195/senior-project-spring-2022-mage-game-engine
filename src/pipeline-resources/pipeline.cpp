#include "pipeline.hpp"
#include "../object-resources/model.hpp"

#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>

using namespace mage;

// Constructor
GraphicsPipeline::GraphicsPipeline(DeviceHandling& device_pass, PipelineInfo& config_info) : device{device_pass} {	
	std::cout << std::endl << "=== GRAPHICS PIPELINE CREATION ===" << std::endl;
	create_pipeline(config_info);
	std::cout << "=== GRAPHICS PIPELINE CREATION SUCCESSFUL ===" << std::endl << std::endl;
}

// Put together graphics pipeline
void GraphicsPipeline::create_pipeline(const PipelineInfo config_info){
	std::cout << "Attempting to create GraphicsPipeline..." << std::endl;

	std::cout << " - reading shader bytecode..." << std::endl;
	// Read bytecode from shaders and create Vulkan modules for them
	auto vertex_bytecode = read_file("src/shaders/vert.spv");
	auto fragment_bytecode = read_file("src/shaders/frag.spv");

	std::cout << " - creating shader modules..." << std::endl;
	vertex_module = create_module(vertex_bytecode);
	fragment_module = create_module(fragment_bytecode);

	// Fillout Vulkan object info regarding shader modules
	std::cout << " - reading in shader information structures..." << std::endl;
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

	std::cout << " - vertex input info structure..." << std::endl;
	auto binding_descriptions = GameModel::Vertex::get_binding_descriptions();
  	auto attribute_descriptions = GameModel::Vertex::get_attribute_descriptions();
  	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
  	vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descriptions.size());
  	vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();
  	vertex_input_info.pVertexBindingDescriptions = binding_descriptions.data();

	// Now to put it all together...
	std::cout << " - attempting to bring together pipeline information..." << std::endl;
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
	pipe_info.basePipelineIndex = -1;
	pipe_info.basePipelineHandle = VK_NULL_HANDLE;

	// Moment of truth
	std::cout << " - final creation of pipeline..." << std::endl;
	 if (vkCreateGraphicsPipelines(device.get_device(), VK_NULL_HANDLE, 1, &pipe_info, nullptr, &graphics_pipeline) != VK_SUCCESS){
	 	std::cerr << "Failed to create graphics pipeline";
	 	exit(EXIT_FAILURE);
	 }

	 std::cout << " - pipeline construction complete!" << std::endl;

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
	std::cout << "   - attempting to create module for shader..." << std::endl;
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
	std::cout << "   - shader module creation successful!" << std::endl;
	return shader_module;
}



void GraphicsPipeline::default_pipeline_info(PipelineInfo &config_info){
	std::cout << "   - input_assembly_info..." << std::endl;
	config_info.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	config_info.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

	std::cout << "   - viewport_info..." << std::endl;
	config_info.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	config_info.viewport_info.viewportCount = 1;
	config_info.viewport_info.pViewports = nullptr;
	config_info.viewport_info.scissorCount = 1;
	config_info.viewport_info.pScissors = nullptr;

	std::cout << "   - rasterization_info..." << std::endl;
	config_info.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	config_info.rasterization_info.depthClampEnable = VK_FALSE;
	config_info.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
	config_info.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
	config_info.rasterization_info.lineWidth = 1.0f;
	config_info.rasterization_info.cullMode = VK_CULL_MODE_NONE;
	config_info.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	config_info.rasterization_info.depthBiasEnable = VK_FALSE;
	config_info.rasterization_info.depthBiasConstantFactor = 0.0f;
  	config_info.rasterization_info.depthBiasClamp = 0.0f;         
  	config_info.rasterization_info.depthBiasSlopeFactor = 0.0f;   
	
	std::cout << "   - multisample_info..." << std::endl;
	config_info.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	config_info.multisample_info.sampleShadingEnable = VK_FALSE;
	config_info.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	config_info.multisample_info.minSampleShading = 1.0f;         
  	config_info.multisample_info.pSampleMask = nullptr;             
  	config_info.multisample_info.alphaToCoverageEnable = VK_FALSE;  
  	config_info.multisample_info.alphaToOneEnable = VK_FALSE;     

	std::cout << "   - color_blend_attachment..." << std::endl;
	config_info.color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	config_info.color_blend_attachment.blendEnable = VK_FALSE;
  	config_info.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  
  	config_info.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  
  	config_info.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              
  	config_info.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   
  	config_info.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  
  	config_info.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              
	
	std::cout << "   - color_blend_info..." << std::endl;
	config_info.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	config_info.color_blend_info.logicOpEnable = VK_FALSE;
	config_info.color_blend_info.logicOp = VK_LOGIC_OP_COPY; 
	config_info.color_blend_info.attachmentCount = 1;
	config_info.color_blend_info.pAttachments = &config_info.color_blend_attachment;
	config_info.color_blend_info.blendConstants[0] = 0.0f;  
  	config_info.color_blend_info.blendConstants[1] = 0.0f;  
  	config_info.color_blend_info.blendConstants[2] = 0.0f;  
  	config_info.color_blend_info.blendConstants[3] = 0.0f;  

	std::cout << "   - depth_stencil_info..." << std::endl;
	config_info.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	config_info.depth_stencil_info.depthTestEnable = VK_TRUE;
	config_info.depth_stencil_info.depthWriteEnable = VK_TRUE;
	config_info.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	config_info.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	config_info.depth_stencil_info.minDepthBounds = 0.0f; 
  	config_info.depth_stencil_info.maxDepthBounds = 1.0f;  
	config_info.depth_stencil_info.stencilTestEnable = VK_FALSE;
	config_info.depth_stencil_info.front = {};  
  	config_info.depth_stencil_info.back = {};   
}

void GraphicsPipeline::bind(VkCommandBuffer command_buffer) {
  	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
}


GraphicsPipeline::~GraphicsPipeline(){
  	vkDestroyShaderModule(device.get_device(), vertex_module, nullptr);
  	vkDestroyShaderModule(device.get_device(), fragment_module, nullptr);
  	vkDestroyPipeline(device.get_device(), graphics_pipeline, nullptr);
}
