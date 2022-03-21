#pragma once

#include "../window-resources/window.hpp"
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
		public:
			GraphicsPipeline(Window& window_pass);
			~GraphicsPipeline();	
			void create_pipeline();
			static std::vector<char> read_file(const std::string& file_name);
			VkShaderModule create_module(const std::vector<char>& data);
			void create_viewport(VkExtent2D swap_extent);
			void config_rasterizer();
			void change_rasterizer_mode(Window window_pass, VkPolygonMode poly);
	};

}
