#pragma once

#include "device.hpp"
#include <string>
#include <vector>

namespace mage {
	
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
			VkPipelineColorBlendStateCreateInfo color_info{};
			VkPipeline graphics_pipeline;
		public:
			GraphicsPipeline(DeviceHandling device_pass);
			~GraphicsPipeline();	
			void create_pipeline(VkFormat format);
			static std::vector<char> read_file(const std::string& file_name);
			VkShaderModule create_module(const std::vector<char>& data);
			void create_viewport(VkExtent2D swap_extent);
			void config_rasterizer();
			void change_rasterizer_mode(Window window_pass, VkPolygonMode poly);
			void config_blending();
			void config_render_pass(VkFormat format);
	};

}
