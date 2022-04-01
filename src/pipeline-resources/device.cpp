#include "device.hpp"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <unordered_set>
#include <set>
#include <limits>
#include <algorithm>

using namespace mage;


const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

DeviceHandling::DeviceHandling(Window &window_pass) : window(window_pass){
	init_vulkan_instance();
	create_surface();
	select_hardware();
	logical_device();
	create_swap_chain();
	create_command_pool();
	create_command_buffer();
}

// Initialize Vulkan library
void DeviceHandling::init_vulkan_instance(){

	// Creating data regarding the application (left pretty blank for now)
	VkApplicationInfo app_data{};
    app_data.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_data;

    // Ensure glfw extensions are present
    uint32_t num_glfw_extensions = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&num_glfw_extensions);
    create_info.enabledExtensionCount = num_glfw_extensions;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledLayerCount = 0;

    // Attempt to create instance
    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
	    std::cerr << "failed to create instance";
	    exit(EXIT_FAILURE);
	}

	check_glfw_extensions();

}


// Search and select necessary graphics card from system
void DeviceHandling::select_hardware(){
	uint32_t num_devices = 0;
	vkEnumeratePhysicalDevices(instance, &num_devices, nullptr);

	if (num_devices == 0){
		std::cerr << "Error finding hardware.\n";
		exit(EXIT_FAILURE);
	}

	//This area can be fleshed out in the future to select optimal GPU when multiple are present
	VkPhysicalDeviceProperties properties;
	std::vector<VkPhysicalDevice> devices(num_devices);
	vkEnumeratePhysicalDevices(instance, &num_devices, devices.data());
	std::cout << "Found " << num_devices << " device(s)" << std::endl;
	for (const auto& device : devices) {
		vkGetPhysicalDeviceProperties(device, &properties);
		std::cout << "Currently checking " << properties.deviceName << "..." << std::endl;
		if (suitable_device(device)) {
            		card = device;
            		break;
        	}
	}

	if (card == nullptr) {
	    std::cerr << "Error finding capable hardware.\n";
		exit(EXIT_FAILURE);
	} 

	vkGetPhysicalDeviceProperties(card, &properties);
	std::cout << "Selected device: " << properties.deviceName << std::endl;

}


void DeviceHandling::check_glfw_extensions(){
	uint32_t num_extensions = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, nullptr);
	std::vector<VkExtensionProperties> extensions(num_extensions);
	vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, extensions.data());
	std::cout << "Available Extensions:" << std::endl;
	std::unordered_set<std::string> list;
	for (const auto &extension : extensions){
		std::cout << " - " << extension.extensionName << std::endl;
		list.insert(extension.extensionName);
	}

	bool quit = false;
	std::cout << "Required Extensions" << std::endl;
	auto required_extensions = get_required_extensions();
	for (const auto &required_extension : required_extensions){
		std::cout << " - " << required_extension << std::endl;
		if (list.find(required_extension) == list.end()) {
			std::cerr << "Missing required glfw extension: " << required_extension << std::endl;
			quit = true;
		}
	}

	if (quit){
		exit(EXIT_FAILURE);
	}


}


std::vector<const char*> DeviceHandling::get_required_extensions(){
	uint32_t num_extensions = 0;
	const char **glfw_extensions;
	glfw_extensions = glfwGetRequiredInstanceExtensions(&num_extensions);
	std::vector<const char *> extensions(glfw_extensions, glfw_extensions + num_extensions);
	return extensions;
}


// Hub for various tests regarding device capabilities
bool DeviceHandling::suitable_device(VkPhysicalDevice device){
	QueueIndices indices = find_families(device);
	bool extensions_supported = check_extension_support(device);
	std::cout << "Device has all required extensions" << std::endl;

	bool swap_support = false;
	if (extensions_supported) {
		SwapChainSupport support = query_support(device);
		swap_support = !support.formats.empty() && !support.present_modes.empty();
	}

	return indices.complete() && swap_support && extensions_supported;
}


// Check if swap chain is supported by selected device
bool DeviceHandling::check_extension_support(VkPhysicalDevice device) {
	uint32_t extension_count;
   	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> extensions_available(extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, extensions_available.data());
	std::set<std::string> extensions_required(device_extensions.begin(), device_extensions.end());
	for (const auto& extension : extensions_available) {
		extensions_required.erase(extension.extensionName);
	}

	return extensions_required.empty();
}

// Find queues supported by selected device
QueueIndices DeviceHandling::find_families(VkPhysicalDevice device) {
    QueueIndices indices;

    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, nullptr);

    std::vector<VkQueueFamilyProperties> families(family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, families.data());

    int i = 0;
    for (const auto &queue_family : families){
    	if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
            indices.graphics_family_has_value = true;
        }

        VkBool32 presentSupport = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

	if (queue_family.queueCount > 0 && presentSupport) {
		indices.present_family = i;
		indices.present_family_has_value = true;
	}

        if (indices.complete()) {
            break;
        }
        i++;
    }

    return indices;
}


// Interfaces physical device with queues
void DeviceHandling::logical_device(){
	QueueIndices indices = find_families(card);

	// Handled in a loop to account for multiple possible queues
	std::vector<VkDeviceQueueCreateInfo> create_info_queue{};
	std::set<uint32_t> unique_queue_families = {indices.graphics_family, indices.present_family.value()};
    float queue_priority = 1.0f;
    for(uint32_t queue_family : unique_queue_families){
    	VkDeviceQueueCreateInfo create_new_info{};
    	create_new_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    	create_new_info.queueFamilyIndex = queue_family;
    	create_new_info.queueCount = 1;
    	create_new_info.pQueuePriorities = &queue_priority;
    	create_info_queue.push_back(create_new_info);
    }

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(create_info_queue.size());
    create_info.pQueueCreateInfos = create_info_queue.data();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();

    if (vkCreateDevice(card, &create_info, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphics_family, 0, &graphics_queue);
}


// Attempts to create surface to connect Vulkan to window
// Using GLFW API for maximum cross-platform support
void DeviceHandling::create_surface() {
	window.create_surface(instance, &surface);
}


// Populate SwapChainSupport struct
SwapChainSupport DeviceHandling::query_support(VkPhysicalDevice device) {
	SwapChainSupport details;

	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
	if (format_count != 0) {
		details.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
	}

	uint32_t present_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_count, nullptr);
	if (present_count != 0) {
		details.present_modes.resize(present_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_count, details.present_modes.data());
	}

	return details;
}


// Settings for color depth and surface format
VkSurfaceFormatKHR DeviceHandling::choose_swap_format(const std::vector<VkSurfaceFormatKHR>& formats) {
	for (const auto& format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	return formats[0];
}


// Choose swapping conditions under presentation mode
VkPresentModeKHR DeviceHandling::choose_swap_mode(const std::vector<VkPresentModeKHR>& modes) {
	// Choose one of the following:
	return VK_PRESENT_MODE_IMMEDIATE_KHR; 	 // Immediate rendering, possible tearing
	// return VK_PRESENT_MODE_FIFO_KHR;	 // Put images into queue when not full
	// return VK_PRESENT_MODE_FIFO_RELAXED_KHR; // Similar to previous mode, but slightly faster with possible tearing
	// return VK_PRESENT_MODE_MAILBOX_KHR;	 // Another variation, resource intensive but utilizes triple buffer for 'best quality'
}


// Choose extent of swap, setting resolution of images in swap)
VkExtent2D DeviceHandling::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
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


// Fully create swap chain
void DeviceHandling::create_swap_chain() {
	// Run the previously defined functions for desired settings
	SwapChainSupport swap_support = query_support(card);
	VkSurfaceFormatKHR surface_format = choose_swap_format(swap_support.formats);
	VkPresentModeKHR present_mode = choose_swap_mode(swap_support.present_modes);
	VkExtent2D present_extent = choose_swap_extent(swap_support.capabilities);

	uint32_t image_count = swap_support.capabilities.minImageCount+1;
	if (swap_support.capabilities.maxImageCount > 0 && image_count > swap_support.capabilities.maxImageCount) {
		image_count = swap_support.capabilities.maxImageCount;
	}

	// Creating swap_chain info
	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = present_extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueIndices indices = find_families(card);
	uint32_t queue_indices[] = {indices.graphics_family, indices.present_family.value()};
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

	if (vkCreateSwapchainKHR(device, &create_info, nullptr, &swap_chain) != VK_SUCCESS) {
		std::cerr << "failed to create swap chain";
	    	exit(EXIT_FAILURE);		
	}

	vkGetSwapchainImagesKHR(device, swap_chain, &image_count, nullptr);
	swap_images.resize(image_count);
	vkGetSwapchainImagesKHR(device, swap_chain, &image_count, swap_images.data());
	swap_image_format = surface_format.format;
	swap_extent = swap_extent;

}


// Create a view onto the image for VkImage
void DeviceHandling::create_image_views() {
	swap_image_views.resize(swap_images.size());

	for (size_t i = 0; i < swap_images.size(); i++) {
		VkImageViewCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = swap_images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = swap_image_format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &create_info, nullptr, &swap_image_views[i]) != VK_SUCCESS) {	
			std::cerr << "failed to create swap chain";
	    		exit(EXIT_FAILURE);		
		}
	}
}

void DeviceHandling::create_command_pool(){
	QueueIndices indices = find_families(card);

	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = indices.graphics_family;
	pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS){
		std::cerr << "failed to create command pool" << std::endl;
		exit(EXIT_FAILURE);
	}

}

void DeviceHandling::create_command_buffer(){
	VkCommandBufferAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocate_info.commandPool = command_pool;
	allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocate_info.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(device, &allocate_info, &command_buffer) != VK_SUCCESS){
		std::cerr << "Failed to allocate command buffer" << std::endl;
		exit(EXIT_FAILURE);
	}
}

// This will later be called in draw_frame
void DeviceHandling::record_command_buffer(VkCommandBuffer command_buffer, uint32_t image_index, VkRenderPass render_pass){
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	
	if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) { 
		std::cerr << "Failed to begin recording command buffer" << std::endl;
		exit(EXIT_FAILURE);
	}

	VkRenderPassBeginInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = render_pass;

}

VkDevice DeviceHandling::get_device(){
	return device;
}

std::vector<VkImageView> DeviceHandling::get_swap_view(){
	return swap_image_views;
}

// Free resources after closed window
DeviceHandling::~DeviceHandling(){
	for (auto image_view : swap_image_views) {
		vkDestroyImageView(device, image_view, nullptr);
	}

	vkDestroyInstance(instance, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
	vkDestroySwapchainKHR(device, swap_chain, nullptr);
}
