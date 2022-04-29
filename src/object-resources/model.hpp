#pragma once

#include "../pipeline-resources/device.hpp"
#include "../pipeline-resources/swapchain.hpp"
#include <vector>
#include <glm/glm.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace mage{

	class GameModel{
		private:
			DeviceHandling &device;
			VkBuffer vertex_buffer;
			VkDeviceMemory vertex_buffer_memory;
			uint32_t vertex_count;
		public:
			struct Vertex {
				glm::vec3 position;
				glm::vec3 color;
				static std::vector<VkVertexInputBindingDescription> get_binding_descriptions(){
					std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
					binding_descriptions[0].binding = 0;
					binding_descriptions[0].stride = sizeof(Vertex);
					binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
					return binding_descriptions;
				}
				static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions(){
					std::vector<VkVertexInputAttributeDescription> attribute_descriptions(2);
					attribute_descriptions[0].binding = 0;
					attribute_descriptions[0].location = 0;
					attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
					attribute_descriptions[0].offset = offsetof(Vertex, position);

					attribute_descriptions[0].binding = 0;
					attribute_descriptions[0].location = 1;
					attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
					attribute_descriptions[0].offset = offsetof(Vertex, color);
					return attribute_descriptions;
				}
			};

			GameModel(DeviceHandling &device_pass, const std::vector<Vertex> &vertices);
			~GameModel();
			void bind(VkCommandBuffer command_buffer);
			void draw(VkCommandBuffer command_buffer);
			void create_vertex_buffers(const std::vector<Vertex> &vertices);
	};

}