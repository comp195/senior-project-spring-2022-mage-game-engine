#include "test-game.hpp"
#include <iostream>

using namespace mage;

TestGame::TestGame() {
// Placeholder constructor
}

// Run window until user wants to close it
void TestGame::run() {
	while(!test_game.close_window()){
		glfwPollEvents();
		draw_frame();
	}
	vkDeviceWaitIdle(test_device.get_device());
}

// This will later be called in draw_frame
void TestGame::record_command_buffer(uint32_t image_index){
 
	std::cout << "Recording command buffer..." << std::endl;
	VkCommandBufferAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocate_info.commandPool = test_device.get_command_pool();
	allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocate_info.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(test_device.get_device(), &allocate_info, &command_buffer) != VK_SUCCESS){
		std::cerr << "Failed to allocate command buffer" << std::endl;
		exit(EXIT_FAILURE);
	}

	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	
	if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) { 
		std::cerr << "Failed to begin recording command buffer" << std::endl;
		exit(EXIT_FAILURE);
	}

	VkRenderPassBeginInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = test_pipeline.get_render_pass();
	render_pass_info.framebuffer = test_pipeline.get_swap_framebuffers(image_index);
	render_pass_info.renderArea.offset = {0, 0};
	render_pass_info.renderArea.extent = test_device.get_swap_extent();

	VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	render_pass_info.clearValueCount = 1;
	render_pass_info.pClearValues = &clear_color;

	vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, test_pipeline.get_pipeline());
	vkCmdDraw(command_buffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(command_buffer);

	if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS){
		std::cerr << "Failed to record command buffer" << std::endl;
		exit(EXIT_FAILURE);
	}

}

void TestGame::sync_objects(){
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(test_device.get_device(), &semaphore_info, nullptr, &image_available_semaphores) != VK_SUCCESS ||
		vkCreateSemaphore(test_device.get_device(), &semaphore_info, nullptr, &render_available_semaphores) != VK_SUCCESS ||
		vkCreateFence(test_device.get_device(), &fence_info, nullptr, &in_flight_fences) != VK_SUCCESS){
		std::cerr << "Failed to sync objects for a frame" << std::endl;
	}
}

void TestGame::draw_frame(){
	std::cout << "Attempting to draw frame..." << std::endl;

	std::cout << " - syncing objects..." << std::endl;
	sync_objects();

	std::cout << " - awaiting and reseting fences..." << std::endl;
	vkWaitForFences(test_device.get_device(), 1, &in_flight_fences, VK_TRUE, UINT64_MAX);
	vkResetFences(test_device.get_device(), 1, &in_flight_fences);

	std::cout << " - acquiring next image..." << std::endl;
	uint32_t image_index;
	vkAcquireNextImageKHR(test_device.get_device(), test_device.get_swap_chain(), UINT64_MAX, image_available_semaphores, VK_NULL_HANDLE, &image_index);
	
	std::cout << " - reseting command buffer..." << std::endl;
	vkResetCommandBuffer(command_buffer, 0);

	std::cout << " - recording command buffer..." << std::endl;
	record_command_buffer(image_index);

	std::cout << " - creating submit_info struct..." << std::endl;
	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore wait_semaphores[] = {image_available_semaphores};
	VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	VkSemaphore signal_semaphores[] = {render_available_semaphores};
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	std::cout << " - attempting to submit draw buffer..." << std::endl;
	if (vkQueueSubmit(test_device.get_graphics_queue(), 1, &submit_info, in_flight_fences) != VK_SUCCESS) {
		std::cerr << "Failed to submit draw buffer" << std::endl;
	}

	VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swap_chains[] = {test_device.get_swap_chain()};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;

    present_info.pImageIndices = &image_index;

    vkQueuePresentKHR(test_device.get_present_queue(), &present_info);

}

TestGame::~TestGame() {
	vkDestroySemaphore(test_device.get_device(), image_available_semaphores, nullptr);
    vkDestroySemaphore(test_device.get_device(), render_available_semaphores, nullptr);
    vkDestroyFence(test_device.get_device(), in_flight_fences, nullptr);
}
