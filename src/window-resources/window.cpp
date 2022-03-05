#include "window.hpp"
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <cstdlib>
#include <vector>
#include <unordered_set>
#include <set>

using namespace mage;


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
		throw std::runtime_error("failed to create instance!");
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


bool Window::suitable_device(VkPhysicalDevice device){
	QueueIndices indices = find_families(device);
	return indices.complete();
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
    create_info.enabledExtensionCount = 0;

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


// Reflects whether user is attempting to currently close window
bool Window::close_window(){
	return glfwWindowShouldClose(window);
}


// Free resources after closed window
Window::~Window(){
	vkDestroyInstance(instance, nullptr);
	//vkDestroyInstance(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}
