#pragma once

#include "../window-resources/window.hpp"
#include <string>
#include <vector>

namespace mage {
	
	class GraphicsPipeline {
		public:
			GraphicsPipeline();
			~GraphicsPipeline();	
		private:
			void create_pipeline();
			static std::vector<char> read_file(const std::string& file_name);
			VkShaderModule create_module(const std::vector<char>& data);
			VkShaderModule vertex_module;
			VkShaderModule fragment_module;
	};

}
