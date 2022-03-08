#include "window.hpp"
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <cstdlib>
#include <vector>
#include <unordered_set>
#include <set>
#include <limits>
#include <algorithm>

using namespace mage;


const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

// Window constructor
Window::Window(int w, int h, std::string title){
	window_width = w;
	window_height = h;
	window_title = title;
	init_window();
	init_vulkan_instance();
	create_surface();
	select_hardware(); 
	logical_device();
	create_swap_chain();
}


// Creation of window via GLFW
void Window::init_window(){
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);
}


// Initialize Vulkan library
void Window::init_vulkan_instance(){

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
}


// Search and select necessary graphics card from system
void Window::select_hardware(){
	uint32_t num_devices = 0;
	vkEnumeratePhysicalDevices(instance, &num_devices, nullptr);

	if (num_devices == 0){
		std::cerr << "Error finding hardware.\n";
		exit(EXIT_FAILURE);
	}

	//This area can be fleshed out in the future to select optimal GPU when multiple are present
	std::vector<VkPhysicalDevice> devices(num_devices);
	vkEnumeratePhysicalDevices(instance, &num_devices, devices.data());
	for (const auto& device : devices) {
		if (suitable_device(device)) {
            		card = device;
            		break;
        	}
	}

	if (card == nullptr) {
	    std::cerr << "Error finding capable hardware.\n";
		exit(EXIT_FAILURE);
	}
}


// Hub for various tests regarding device capabilities
bool Window::suitable_device(VkPhysicalDevice device){
	QueueIndices indices = find_families(device);
	bool extensions_supported = check_extension_support(device);
	bool swap_support = false;
	if (extensions_supported) {
		SwapChainSupport support = query_support(device);
		swap_support = !support.formats.empty() && !support.present_modes.empty();
	}

	return indices.complete() && extensions_supported && swap_support;
}


// Check if swap chain is supported by selected device
bool Window::check_extension_support(VkPhysicalDevice device) {
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
QueueIndices Window::find_families(VkPhysicalDevice device) {
    QueueIndices indices;

    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, nullptr);

    std::vector<VkQueueFamilyProperties> families(family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, families.data());

    for (int i = 0; i < families.size(); i++){
    	if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
		    indices.present_family = i;
		}

        if (indices.complete()) {
            break;
        }
    }

    return indices;
}


// Interfaces physical device with queues
void Window::logical_device(){
	QueueIndices indices = find_families(card);

	// Handled in a loop to account for multiple possible queues
	std::vector<VkDeviceQueueCreateInfo> create_info_queue{};
	std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};
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
    VkPhysicalDeviceFeatures device_features{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(create_info_queue.size());
    create_info.pQueueCreateInfos = create_info_queue.data();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();

    if (vkCreateDevice(card, &create_info, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
}


// Attempts to create surface to connect Vulkan to window
// Using GLFW API for maximum cross-platform support
void Window::create_surface() {
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}


// Populate SwapChainSupport struct
SwapChainSupport Window::query_support(VkPhysicalDevice device) {
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
VkSurfaceFormatKHR Window::choose_swap_format(const std::vector<VkSurfaceFormatKHR>& formats) {
	for (const auto& format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	return formats[0];
}


// Choose swapping conditions under presentation mode
VkPresentModeKHR Window::choose_swap_mode(const std::vector<VkPresentModeKHR>& modes) {
	// Choose one of the following:
	return VK_PRESENT_MODE_IMMEDIATE_KHR; 	 // Immediate rendering, possible tearing
	// return VK_PRESENT_MODE_FIFO_KHR;	 // Put images into queue when not full
	// return VK_PRESENT_MODE_FIFO_RELAXED_KHR; // Similar to previous mode, but slightly faster with possible tearing
	// return VK_PRESENT_MODE_MAILBOX_KHR;	 // Another variation, resource intensive but utilizes triple buffer for 'best quality'
}


// Choose extent of swap, setting resolution of images in swap)
VkExtent2D Window::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		VkExtent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actual_extent;
	}
}


// Fully create swap chain
void Window::create_swap_chain() {
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
	uint32_t queue_indices[] = {indices.graphics_family.value(), indices.present_family.value()};
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
void Window::create_image_views() {
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


// Reflects whether user is attempting to currently close window
bool Window::close_window(){
	return glfwWindowShouldClose(window);
}


// Free resources after closed window
Window::~Window(){
	for (auto image_view : swap_image_views) {
		vkDestroyImageView(device, image_view, nullptr);
	}

	vkDestroyInstance(instance, nullptr);
	//vkDestroyInstance(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
	vkDestroySwapchainKHR(device, swap_chain, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}
