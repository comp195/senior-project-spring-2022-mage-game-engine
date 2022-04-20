#pragma once

#include "device.hpp"
#include <string>
#include <vector>

namespace mage {

	struct PipelineInfo {
		VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
		VkPipelineRasterizationStateCreateInfo rasterization_info;
		VkPipelineMultisampleStateCreateInfo multisample_info;
		VkPipelineColorBlendAttachmentState color_blend_attachment;
		VkPipelineColorBlendStateCreateInfo color_blend_info;
		VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
		VkPipelineLayout pipeline_layout = nullptr;
		VkRenderPass render_pass = nullptr;
		VkViewport viewport;
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
			static PipelineInfo default_pipeline_info(VkExtent2D swap_extent);
			void bind(VkCommandBuffer command_buffer);

			VkPipeline get_pipeline(){return graphics_pipeline;}
	};

}
