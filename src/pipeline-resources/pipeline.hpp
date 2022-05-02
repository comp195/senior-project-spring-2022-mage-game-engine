#pragma once

#include "device.hpp"
#include <string>
#include <vector>

namespace mage {

	struct PipelineInfo {
		VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
		VkPipelineViewportStateCreateInfo viewport_info;
		VkPipelineRasterizationStateCreateInfo rasterization_info;
		VkPipelineMultisampleStateCreateInfo multisample_info;
		VkPipelineColorBlendAttachmentState color_blend_attachment;
		VkPipelineColorBlendStateCreateInfo color_blend_info;
		VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
		std::vector<VkDynamicState> dynamic_state_enable;
  		VkPipelineDynamicStateCreateInfo dynamic_state_info;
		VkPipelineLayout pipeline_layout = nullptr;
		VkRenderPass render_pass = nullptr;
		VkRect2D scissor;
		uint32_t subpass = 0;
	};
	
	class GraphicsPipeline {
		private:
			VkShaderModule vertex_module;
			VkShaderModule fragment_module;
			DeviceHandling &device;
			VkPipeline graphics_pipeline;
		public:
			GraphicsPipeline(DeviceHandling& device_pass, PipelineInfo& config_info);
			~GraphicsPipeline();	
			void create_pipeline(const PipelineInfo config_info);
			static std::vector<char> read_file(const std::string& file_name);
			VkShaderModule create_module(const std::vector<char>& data);
			static void default_pipeline_info(PipelineInfo &configInfo);
			void bind(VkCommandBuffer command_buffer);

			VkPipeline get_pipeline(){return graphics_pipeline;}
	};

}
