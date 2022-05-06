#include "model.hpp"

#include <iostream>
#include <cstring>

using namespace mage;

GameModel::GameModel(DeviceHandling &device_pass, const std::vector<Vertex> &vertices) : device{device_pass} {
	std::cout << std::endl << "=== GAME MODEL CREATION ===" << std::endl; 
	create_vertex_buffers(vertices);
	std::cout << "=== GAME MODEL FINISHED ===" << std::endl;
}


void GameModel::create_vertex_buffers(const std::vector<Vertex> &vertices){
	vertex_count = static_cast<uint32_t>(vertices.size());
	VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count;
	device.create_buffer(
	  buffer_size,
	  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	  vertex_buffer,
	  vertex_buffer_memory);
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