#include "window.hpp"
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <cstdlib>
#include <vector>
#include <unordered_set>

using namespace mage;


// Window constructor
Window::Window(int w, int h, std::string title){
	window_width = w;
	window_height = h;
	window_title = title;
	init_window();
	init_vulkan_instance();
	select_hardware(); 
	//logical_device();
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

        if (indices.complete()) {
            break;
        }
    }

    return indices;
}


/*
// Reflects whether user is attempting to currently close window
void Window::logical_device(){
	QueueFamilyIndices indices = findfamilies(physicalDevice);

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;

        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}
*/

// Reflects whether user is attempting to currently close window
bool Window::close_window(){
	return glfwWindowShouldClose(window);
}


// Free resources after closed window
Window::~Window(){
	vkDestroyInstance(instance, nullptr);
	//vkDestroyInstance(device, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}
