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
	std::cout << std::endl << "=== DEVICE HANDLING ===" << std::endl;
	init_vulkan_instance();
	create_surface();
	select_hardware();
	logical_device();
	create_command_pool();
	std:: cout << "=== DEVICE HANDLING SUCCESSFUL ===" << std::endl;
}

// Initialize Vulkan library
void DeviceHandling::init_vulkan_instance(){
	std::cout << "Attempting to initialize Vulkan instance..." << std::endl;

	// Creating data regarding the application (left pretty blank for now)
	std::cout << " - creating application info..." << std::endl;
	VkApplicationInfo app_data{};
    app_data.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_data;

    // Ensure glfw extensions are present
    std::cout << " - ensuring GLFW extensions are present..." << std::endl;
    uint32_t num_glfw_extensions = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&num_glfw_extensions);
    create_info.enabledExtensionCount = num_glfw_extensions;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledLayerCount = 0;

    // Attempt to create instance
    std::cout << " - attempting to create instance..." << std::endl;
    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
	    std::cerr << "failed to create instance";
	    exit(EXIT_FAILURE);
	}
	check_glfw_extensions();
	std::cout << " - Vulkan instance creation successful!" << std::endl;
}


// Search and select necessary graphics card from system
void DeviceHandling::select_hardware(){
	std::cout << "Attempting to locate physical hardware for graphics calculations..." << std::endl;

	std::cout << " - searching for any hardware whatsoever..." << std::endl;
	uint32_t num_devices = 0;
	vkEnumeratePhysicalDevices(instance, &num_devices, nullptr);
	if (num_devices == 0){
		std::cerr << "Error finding any hardware.\n";
		exit(EXIT_FAILURE);
	}

	//This area can be fleshed out in the future to select optimal GPU when multiple are present
	std::cout << " - selecting optimal device..." << std::endl;
	VkPhysicalDeviceProperties properties;
	std::vector<VkPhysicalDevice> devices(num_devices);
	vkEnumeratePhysicalDevices(instance, &num_devices, devices.data());
	std::cout << "   - found " << num_devices << " device(s)" << std::endl;
	for (const auto& device : devices) {
		vkGetPhysicalDeviceProperties(device, &properties);
		std::cout << "     - currently checking " << properties.deviceName << "..." << std::endl;
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
	std::cout << " - selected device: " << properties.deviceName << std::endl;

}


void DeviceHandling::check_glfw_extensions(){
	std::cout << "   - checking GLFW extensions..." << std::endl;
	uint32_t num_extensions = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, nullptr);
	std::vector<VkExtensionProperties> extensions(num_extensions);
	vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, extensions.data());
	std::cout << "   - available extensions:" << std::endl;
	std::unordered_set<std::string> list;
	for (const auto &extension : extensions){
		std::cout << "     - " << extension.extensionName << std::endl;
		list.insert(extension.extensionName);
	}

	bool quit = false;
	std::cout << "   - required extensions" << std::endl;
	auto required_extensions = get_required_extensions();
	for (const auto &required_extension : required_extensions){
		std::cout << "     - " << required_extension << std::endl;
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
	std::cout << "Attempting to create logical Vulkan device..." << std::endl;
	QueueIndices indices = find_families(card);

	// Handled in a loop to account for multiple possible queues
	std::cout << " - creating info for info_queue..." << std::endl;
	std::vector<VkDeviceQueueCreateInfo> create_info_queue{};
	std::set<uint32_t> unique_queue_families = {indices.graphics_family, indices.present_family};
    float queue_priority = 1.0f;
    for(uint32_t queue_family : unique_queue_families){
    	VkDeviceQueueCreateInfo create_new_info{};
    	create_new_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    	create_new_info.queueFamilyIndex = queue_family;
    	create_new_info.queueCount = 1;
    	create_new_info.pQueuePriorities = &queue_priority;
    	create_info_queue.push_back(create_new_info);
    }

    std::cout << " - creating info for device..." << std::endl;
    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(create_info_queue.size());
    create_info.pQueueCreateInfos = create_info_queue.data();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();

    std::cout << " - attempting to create device..." << std::endl;
    if (vkCreateDevice(card, &create_info, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    std::cout << " - appending graphics_queue and present_queue..." << std::endl;
    vkGetDeviceQueue(device, indices.graphics_family, 0, &graphics_queue);
    vkGetDeviceQueue(device, indices.present_family, 0, &present_queue);

    std::cout << " - link between physical card and logical device successful!" << std::endl;
}


// Attempts to create surface to connect Vulkan to window
// Using GLFW API for maximum cross-platform support
void DeviceHandling::create_surface() {
	std::cout << "Attempting to connect Vulkan to window surface..." << std::endl;
	window.create_surface(instance, &surface);
	std::cout << " - surface creation successful!" << std::endl;
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


void DeviceHandling::create_command_pool(){
	std::cout << "Attempting to create command pool..." << std::endl;
	QueueIndices indices = find_families(card);
	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = indices.graphics_family;
	pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS){
		std::cerr << "failed to create command pool" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << " - create pool creation successful!" << std::endl;

}

VkFormat DeviceHandling::find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(card, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (
        tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  throw std::runtime_error("failed to find supported format!");
}

// Free resources after closed window
DeviceHandling::~DeviceHandling(){
	vkDestroyInstance(instance, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyCommandPool(device, command_pool, nullptr);
}
