#include "artist.hpp"

#include <iostream>
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <chrono>

using namespace mage;

DrawHandling::DrawHandling(Window &window_pass, DeviceHandling &device_pass) : window{window_pass}, device{device_pass} {
  std::cout << std::endl << "=== ARTIST HANDLING START ===" << std::endl;
  create_swapchain();
  create_command_buffer();
  std::cout << "=== ARTIST HANDLING SUCCESSFUL ===" << std::endl;
}

void DrawHandling::create_command_buffer(){
  std::cout << "Attempting to create command buffer..." << std::endl;
  std::cout << " - resizing commnad buffer..." << std::endl;
  command_buffer.resize(swapchain->get_max_frames());

  std::cout << " - creating info for buffer allocation..." << std::endl;
  VkCommandBufferAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandPool = device.get_command_pool();
  allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffer.size());

  std::cout << " - attempting to allocate command buffer..." << std::endl;
  if (vkAllocateCommandBuffers(device.get_device(), &allocate_info, command_buffer.data()) != VK_SUCCESS) {
    std::cerr << "Failed to allocate command buffer";
    exit(EXIT_FAILURE);
  }
	std::cout << " - command buffer creation successful!" << std::endl;
}

void DrawHandling::create_swapchain() {
  std::cout << "Attempting to create swapchain..." << std::endl;
  auto extent = window.get_extent();
  while (extent.width == 0 || extent.height == 0) {
    extent = window.get_extent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(device.get_device());

  if (swapchain == nullptr) {
    swapchain = std::make_unique<SwapChainHandling>(device, extent);
  } else {
    std::shared_ptr<SwapChainHandling> oldSwapChain = std::move(swapchain);
    swapchain = std::make_unique<SwapChainHandling>(device, extent, oldSwapChain);

    if (!oldSwapChain->compare_swap_formats(*swapchain.get())) {
      throw std::runtime_error("Swap chain image(or depth) format has changed!");
    }
  }
  std::cout << " - swap chain creation successful!" << std::endl;
}

VkCommandBuffer DrawHandling::draw_start(){
  std::cout << "Attempting to acquire next image..." << std::endl;
  auto result = swapchain->acquire_next_image(&current_image);
  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    std::cerr << "Failed to acquire next image" << std::endl;
  }

  frame_started = true;
  auto current_command_buffer = get_current_command_buffer();
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  if (vkBeginCommandBuffer(current_command_buffer, &beginInfo) != VK_SUCCESS) {
    std::cerr << "Failed to begin creating command_buffer" << std::endl;
  }

  return current_command_buffer;
}

void DrawHandling::draw_end(){
  auto current_command_buffer = get_current_command_buffer();
  if (vkEndCommandBuffer(current_command_buffer) != VK_SUCCESS) {
    std::cerr << "Failed to end command buffer" << std::endl;
  }

  auto result = swapchain->submit_command_buffers(&current_command_buffer, &current_image);
  if (result != VK_SUCCESS){
    std::cerr << "Failed to present image to swap chain";
    exit(EXIT_FAILURE);
  }

  frame_started = false;
  current_frame = (current_frame + 1) % swapchain->get_max_frames();
}

void DrawHandling::swapchain_render_start(VkCommandBuffer current_command_buffer){
  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = swapchain->get_render_pass();
  render_pass_info.framebuffer = swapchain->get_framebuffers(current_image);

  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = swapchain->get_swap_extent();

  std::array<VkClearValue, 2> clear_values{};
  clear_values[0].color = {0.2f, 0.2f, 0.2f, 1.0f};
  clear_values[1].depthStencil = {1.0f, 0};
  render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
  render_pass_info.pClearValues = clear_values.data();

  vkCmdBeginRenderPass(current_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapchain->get_swap_extent().width);
  viewport.height = static_cast<float>(swapchain->get_swap_extent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, swapchain->get_swap_extent()};
  vkCmdSetViewport(current_command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(current_command_buffer, 0, 1, &scissor);
}

void DrawHandling::swapchain_render_end(VkCommandBuffer current_command_buffer){
  vkCmdEndRenderPass(current_command_buffer);
}


DrawHandling::~DrawHandling() {
	vkFreeCommandBuffers(device.get_device(), device.get_command_pool(), static_cast<uint32_t>(command_buffer.size()), command_buffer.data());
  command_buffer.clear();
}
