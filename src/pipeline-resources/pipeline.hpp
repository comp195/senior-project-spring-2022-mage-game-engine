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
		public:
			GraphicsPipeline(VkDevice device_pass);
			~GraphicsPipeline();	
			void create_pipeline();
			static std::vector<char> read_file(const std::string& file_name);
			VkShaderModule create_module(const std::vector<char>& data);
	};

}
