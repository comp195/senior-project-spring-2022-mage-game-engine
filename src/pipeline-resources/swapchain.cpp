#include "swapchain.hpp"
#include <iostream>

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <vector>
#include <unordered_set>
#include <algorithm>

using namespace mage;


SwapChainHandling::SwapChainHandling(DeviceHandling &device_pass, VkExtent2D extent_pass) : device{device_pass}, window_extent{extent_pass} {
	std::cout << std::endl << "=== SWAP CHAIN HANDLING ===" << std::endl;
	create_swap_chain();
	create_image_views();
	create_render_pass();
	create_depth_resources();
	create_framebuffers();
	create_sync_objects();	
	std:: cout << "=== SWAP CHAIN HANDLING SUCCESSFUL ===" << std::endl;
}


// Fully create swap chain
void SwapChainHandling::create_swap_chain() {
	std::cout << "Attempting to create swap chain..." << std::endl;
	// Run the previously defined functions for desired settings

	std::cout << " - choosing format, mode, and extent..." << std::endl;
	SwapChainSupport swap_support = device.get_swap_chain_support();
	VkSurfaceFormatKHR surface_format = choose_swap_format(swap_support.formats);
	VkPresentModeKHR present_mode = choose_swap_mode(swap_support.present_modes);
	VkExtent2D present_extent = choose_swap_extent(swap_support.capabilities);

	std::cout << " - getting image count..." << std::endl;
	uint32_t image_count = swap_support.capabilities.minImageCount+1;
	if (swap_support.capabilities.maxImageCount > 0 && image_count > swap_support.capabilities.maxImageCount) {
		image_count = swap_support.capabilities.maxImageCount;
	}

	std::cout << " - creating swap chain info..." << std::endl;
	// Creating swap_chain info
	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = device.get_surface();
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = window_extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	std::cout << " - getting queue indices..." << std::endl;
	QueueIndices indices = device.get_queue_families();
	uint32_t queue_indices[] = {indices.graphics_family, indices.present_family};
	if (indices.graphics_family != indices.present_family) {
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_indices;
	} else {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
	}

	create_info.preTransform = swap_support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	std::cout << " - creating swap chain..." << std::endl;
	if (vkCreateSwapchainKHR(device.get_device(), &create_info, nullptr, &swap_chain) != VK_SUCCESS) {
		std::cerr << "failed to create swap chain";
	    	exit(EXIT_FAILURE);		
	}

	vkGetSwapchainImagesKHR(device.get_device(), swap_chain, &image_count, nullptr);
	swap_images.resize(image_count);
	vkGetSwapchainImagesKHR(device.get_device(), swap_chain, &image_count, swap_images.data());
	swap_image_format = surface_format.format;
	swap_extent = window_extent;

	std::cout << " - swap chain creation successful!" << std::endl;

}


VkSurfaceFormatKHR SwapChainHandling::choose_swap_format(const std::vector<VkSurfaceFormatKHR>& formats) {
	std::cout << "   - choosing swap format..." << std::endl;
	for (const auto& format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}
	return formats[0];
}


// Choose swapping conditions under presentation mode
VkPresentModeKHR SwapChainHandling::choose_swap_mode(const std::vector<VkPresentModeKHR>& modes) {
	std::cout << "   - choosing swap mode..." << std::endl;
	// Choose one of the following:
	return VK_PRESENT_MODE_IMMEDIATE_KHR; 	 // Immediate rendering, possible tearing
	// return VK_PRESENT_MODE_FIFO_KHR;	 // Put images into queue when not full
	// return VK_PRESENT_MODE_FIFO_RELAXED_KHR; // Similar to previous mode, but slightly faster with possible tearing
	// return VK_PRESENT_MODE_MAILBOX_KHR;	 // Another variation, resource intensive but utilizes triple buffer for 'best quality'
}


// Choose extent of swap, setting resolution of images in swap)
VkExtent2D SwapChainHandling::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
	std::cout << "   - choosing swap extent..." << std::endl;
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		int width, height;
		VkExtent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actual_extent;
	}
}


// Create a view onto the image for VkImage
void SwapChainHandling::create_image_views() {
	std::cout << "Attempting to create image views..." << std::endl;
	swap_image_views.resize(swap_images.size());

	for (size_t i = 0; i < swap_images.size(); i++) {
		std::cout << " - creating info for image view..." << std::endl;
		VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = swap_images[i];
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = swap_image_format;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    std::cout << " - attempting to create image view..." << std::endl;
		if (vkCreateImageView(device.get_device(), &create_info, nullptr, &swap_image_views[i]) != VK_SUCCESS) {	
			std::cerr << "failed to create swap chain";
	    		exit(EXIT_FAILURE);		
		}
	}

	std::cout << " - image view creation successful!" << std::endl;

}


void SwapChainHandling::create_render_pass(){
	std::cout << "Attempting to create render pass..." << std::endl;

	std::cout << " - creating info for depth_attachment..." << std::endl;
  VkAttachmentDescription depth_attachment{};
  depth_attachment.format = find_depth_format();
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  std::cout << "   - referencing depth_reference..." << std::endl;
  VkAttachmentReference depth_reference{};
  depth_reference.attachment = 1;
  depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::cout << " - creating info for color_attachment..." << std::endl;
	VkAttachmentDescription color_attachment{};
	color_attachment.format = device.get_swap_format();
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	std::cout << "   - referencing color_reference..." << std::endl;
	VkAttachmentReference color_reference{};
	color_reference.attachment = 0;
	color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	std::cout << " - creating info for subpass..." << std::endl;
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_reference;
	subpass.pDepthStencilAttachment = &depth_reference;

	std::cout << " - creating info for subpass_dependency..." << std::endl;
	VkSubpassDependency subpass_dependency = {};
	subpass_dependency.dstSubpass = 0;
	subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dependency.srcAccessMask = 0;
	subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	std::array<VkAttachmentDescription, 2> attachments = {color_attachment, depth_attachment};

	std::cout << " - creating info for render_pass..." << std::endl;
	VkRenderPassCreateInfo render_info{};
  render_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  render_info.pAttachments = attachments.data();
  render_info.subpassCount = 1;
  render_info.pSubpasses = &subpass;
  render_info.dependencyCount = 1;
  render_info.pDependencies = &subpass_dependency;

  std::cout << " - creating render pass..." << std::endl;
	if (vkCreateRenderPass(device.get_device(), &render_info, nullptr, &render_pass) != VK_SUCCESS){
		std::cerr << "Failed to create render pass";
		exit(EXIT_FAILURE);
	}
	std::cout << " - render pass creation successful!" << std::endl;
}


VkFormat SwapChainHandling::find_depth_format(){
	return device.find_supported_format({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
      															 	 VK_IMAGE_TILING_OPTIMAL,
      															 	 VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}


void SwapChainHandling::create_depth_resources(){
	std::cout << "Attempting to create depth resources..." << std::endl;

	std::cout << " - addressing depth format..." << std::endl;
	VkFormat depth_format = find_depth_format();
  depth_images.resize(swap_images.size());
  depth_images_memories.resize(swap_images.size());
  depth_images_views.resize(swap_images.size());

  for (int i = 0; i < depth_images.size(); i++) {
  	std::cout << " - creating info for image_info..." << std::endl;
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = swap_extent.width;
    image_info.extent.height = swap_extent.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = depth_format;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;
    create_image(image_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_images[i], depth_images_memories[i]);

    std::cout << " - creating info for view_info..." << std::endl;
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = depth_images[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = depth_format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    std::cout << " - creating image view..." << std::endl;
    if (vkCreateImageView(device.get_device(), &view_info, nullptr, &depth_images_views[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create texture image view!");
    }
    std::cout << " - depth resource creation successful!" << std::endl;
  }
}


void SwapChainHandling::create_image(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &image_memory) {
  std::cout << "   - creating image...." << std::endl;
  if (vkCreateImage(device.get_device(), &image_info, nullptr, &image) != VK_SUCCESS) {
    std::cerr << "Failed to create image" << std::endl;
  	exit(EXIT_FAILURE);
  }

  std::cout << "   - finding memory requirements and allocating space..." << std::endl;
  VkMemoryRequirements memory_requirements;
  vkGetImageMemoryRequirements(device.get_device(), image, &memory_requirements);
  VkMemoryAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info.allocationSize = memory_requirements.size;
  allocate_info.memoryTypeIndex = find_memory_type(memory_requirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device.get_device(), &allocate_info, nullptr, &image_memory) != VK_SUCCESS) {
  	std::cerr << "Failed to allocate image memory" << std::endl;
  	exit(EXIT_FAILURE);
  } else if (vkBindImageMemory(device.get_device(), image, image_memory, 0) != VK_SUCCESS) {
    std::cerr << "Failed to bind image memory" << std::endl;
  	exit(EXIT_FAILURE);
  } else {
  	std::cout << "   - image creation and memory allocation successful!" << std::endl;
  }
}


uint32_t SwapChainHandling::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
	std::cout << "     - finding memory type..." << std::endl;
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(device.get_card(), &memory_properties);
  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    if ((type_filter & (1 << i)) &&
        (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
	std::cerr << "Failed to find memory type" << std::endl;
  exit(EXIT_FAILURE);
}


void SwapChainHandling::create_framebuffers(){
	std::cout << "Attempting to create framebuffer..." << std::endl;
	swap_chain_framebuffers.resize(swap_images.size());

	for (size_t i = 0; i < swap_images.size(); i++){
		std::cout << "   - grabbing framebuffer attachments..." << std::endl;
		std::array<VkImageView, 2> attachments = {swap_image_views[i], depth_images_views[i]};
		
		std::cout << "   - creating framebuffer info..." << std::endl;
    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass;
    framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = swap_extent.width;
    framebuffer_info.height = swap_extent.height;
    framebuffer_info.layers = 1;

		std::cout << "   - creating framebuffer..." << std::endl;
		if (vkCreateFramebuffer(device.get_device(), &framebuffer_info, nullptr, &swap_chain_framebuffers[i]) != VK_SUCCESS) {
			std::cerr << "Failed to create framebuffer" << std::endl;
			exit(EXIT_FAILURE);
		}
		std::cout << "   - individual framebuffer creation successful!" << std::endl;
	}
	std::cout << " - total framebuffer creation successful!" << std::endl;
}


void SwapChainHandling::create_sync_objects(){
	std::cout << "Attempting to sync objects..." << std::endl;

	std::cout << " - resizing semaphores and flight-related objects..." << std::endl;
	image_available_semaphores.resize(MAX_FRAMES);
 	render_available_semaphores.resize(MAX_FRAMES);
  in_flight_fences.resize(MAX_FRAMES);
  images_in_flight.resize(swap_images.size(), VK_NULL_HANDLE);

  std::cout << " - creating info for semaphore_info and fence_info..." << std::endl;
  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkFenceCreateInfo fence_info = {};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  std::cout << " - attempting to create semaphores and fences..." << std::endl;
  for (size_t i = 0; i < MAX_FRAMES; i++) {
    if (vkCreateSemaphore(device.get_device(), &semaphore_info, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device.get_device(), &semaphore_info, nullptr, &render_available_semaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device.get_device(), &fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS) {
      		std::cerr << "Failed to sync objects" << std::endl;
      		exit(EXIT_FAILURE);
    }
  }
  std::cout << " - semaphore and fence creation successful!" << std::endl;
}


VkResult SwapChainHandling::acquire_next_image(uint32_t *image_index) {
	std::cout << "     - waiting for fences..." << std::endl;
  vkWaitForFences(device.get_device(), 1, &in_flight_fences[current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max());

  std::cout << "     - acquiring next image..." << std::endl;
  VkResult result = vkAcquireNextImageKHR(device.get_device(), swap_chain,
  																				std::numeric_limits<uint64_t>::max(), image_available_semaphores[current_frame],
      																		VK_NULL_HANDLE, image_index);

  return result;
}


VkResult SwapChainHandling::submit_command_buffers(const VkCommandBuffer *buffers, uint32_t *image_index) {
  std::cout << "Attempting to submit command buffers..." << std::endl;

  std::cout << " - waiting for fences if images in flight..." << std::endl;
  if (images_in_flight[*image_index] != VK_NULL_HANDLE) {
    vkWaitForFences(device.get_device(), 1, &images_in_flight[*image_index], VK_TRUE, UINT64_MAX);
  }

  std::cout << " - setting image in flight to current_frame flight fence..." << std::endl;
  images_in_flight[*image_index] = in_flight_fences[current_frame];

  std::cout << " - creating info for submit_info..." << std::endl;
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {image_available_semaphores[current_frame]};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = waitSemaphores;
  submit_info.pWaitDstStageMask = waitStages;

  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = buffers;

  VkSemaphore signalSemaphores[] = {render_available_semaphores[current_frame]};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signalSemaphores;

  vkResetFences(device.get_device(), 1, &in_flight_fences[current_frame]);
  if (vkQueueSubmit(device.get_graphics_queue(), 1, &submit_info, in_flight_fences[current_frame]) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swap_chains[] = {swap_chain};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swap_chains;
  present_info.pImageIndices = image_index;

  auto result = vkQueuePresentKHR(device.get_present_queue(), &present_info);

  current_frame = (current_frame + 1) % MAX_FRAMES;

  return result;
}


SwapChainHandling::~SwapChainHandling() {

}
