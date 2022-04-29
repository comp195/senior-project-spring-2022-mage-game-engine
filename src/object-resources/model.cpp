#include "model.hpp"

#include <iostream>
#include <cstring>

using namespace mage;

GameModel::GameModel(DeviceHandling &device_pass, const std::vector<Vertex> &vertices) : device{device_pass} {
	create_vertex_buffers(vertices);
}


void GameModel::create_vertex_buffers(const std::vector<Vertex> &vertices){
	vertex_count = static_cast<uint32_t>(vertices.size());
	VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count;

	VkBufferCreateInfo buffer_info{};
	buffer_info.size = buffer_size;
	buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if(vkCreateBuffer(device.get_device(), &buffer_info, nullptr, &vertex_buffer) != VK_SUCCESS){
		std::cerr << "Failed to create vertex buffer";
		exit(EXIT_FAILURE);
	}

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(device.get_device(), vertex_buffer, &memory_requirements);
		
	VkMemoryAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.allocationSize = memory_requirements.size;

	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(device.get_card(), &memory_properties);
  	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    	if ((memory_requirements.memoryTypeBits & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
      		allocate_info.memoryTypeIndex = i;
    	}
  	}

  	if(vkAllocateMemory(device.get_device(), &allocate_info, nullptr, &vertex_buffer_memory) != VK_SUCCESS) {
  		std::cerr << "Failed to allocate vertex buffer memory" << std::endl;
  		exit(EXIT_FAILURE);
  	}

  	vkBindBufferMemory(device.get_device(), vertex_buffer, vertex_buffer_memory, 0);

  	void *data;
  	vkMapMemory(device.get_device(), vertex_buffer_memory, 0, buffer_size, 0, &data);
  	memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
  	vkUnmapMemory(device.get_device(), vertex_buffer_memory);
}

void GameModel::bind(VkCommandBuffer command_buffer){
	VkBuffer buffers[] = {vertex_buffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
}

void GameModel::draw(VkCommandBuffer command_buffer){
	vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
}

GameModel::~GameModel(){
	vkDestroyBuffer(device.get_device(), vertex_buffer, nullptr);
	vkFreeMemory(device.get_device(), vertex_buffer_memory, nullptr);
}