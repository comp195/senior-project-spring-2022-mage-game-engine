#pragma once

#include "device.hpp"
#include <string>
#include <vector>

namespace mage {

	struct PipelineInfo {
		std::vector<VkVertexInputBindingDescription> binding_description{};
		std::vector<VkVertexInputAttributeDescription> attribute_description{};
		VkPipelineViewportStateCreateInfo viewport_info{};
		VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
		VkPipelineRasterizationStateCreateInfo rasterization_info{};
		VkPipelineMultisampleStateCreateInfo multisample_info{};
		VkPipelineColorBlendAttachmentState color_blend_attachment{};
		VkPipelineColorBlendStateCreateInfo color_blend_info{};
		VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
		//VkPipelineDynamicStateCreateInfo dynamic_state{};
		//std::vector<VkDynamicState> dynamic_states[] = {
		//	VK_DYNAMIC_STATE_VIEWPORT,
		//	VK_DYNAMIC_STATE_LINE_WIDTH	
		//};
		VkPipelineLayout pipeline_layout = nullptr;
		VkRenderPass render_pass = nullptr;
		uint32_t subpass = 0;
	};
	
	class GraphicsPipeline {
		private:
			VkShaderModule vertex_module;
			VkShaderModule fragment_module;
			VkDevice device;
			VkViewport viewport;
			VkRect2D scissor{};
			VkPipelineRasterizationStateCreateInfo rasterizer{};
			VkRenderPass render_pass;
			VkPipelineLayout pipeline_layout;
			VkSubpassDescription subpass{};
			VkPipelineColorBlendStateCreateInfo color_info{};
			VkPipeline graphics_pipeline;
			std::vector<VkFramebuffer> swap_chain_framebuffers;
		public:
			GraphicsPipeline(DeviceHandling& device_pass);
			~GraphicsPipeline();	
			void create_pipeline(DeviceHandling& device_pass);
			static std::vector<char> read_file(const std::string& file_name);
			VkShaderModule create_module(const std::vector<char>& data);
			void create_viewport(VkExtent2D swap_extent);
			void config_rasterizer();
			void change_rasterizer_mode(Window window_pass, VkPolygonMode poly);
			void config_blending();
			void config_render_pass(VkFormat format);
			PipelineInfo default_pipeline_info(DeviceHandling& device_pass);
			void create_framebuffer(DeviceHandling& device_pass);
	};

}
