#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#define DBG_CHECKPOINT std::cout << "👍👍DEBUG: All good until here..." << std::endl
#define DBG_ERR(msg) std::cout << "❌❌" msg << std::endl;

/* 
 * for bitmask in bitmasks 
 *  test bitmask in value
 *  add check for bitmask
 *  all good
 */

int main (int argc, char *argv[]) {
    /* 
     * Get version 
     */
    uint32_t version = 0;
    vkEnumerateInstanceVersion(&version);
    std::cout << "Vulkan Instance version: " << version << std::endl;

    /* 
     * Create Vulkan instance
     */
    VkInstance vInstance = {0}; 
    VkResult result = VK_SUCCESS;

    VkInstanceCreateInfo vInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .pApplicationInfo = nullptr,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
    };

    /* Enabling Extension for MacOS Only */
    std::vector<const char*> extNames;
    extNames.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    extNames.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    vInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    vInfo.enabledExtensionCount = static_cast<uint32_t>(extNames.size());
    vInfo.ppEnabledExtensionNames = extNames.data();

    /* Creating the actual Vulkan Instance */
    result = vkCreateInstance(&vInfo, nullptr, &vInstance);

    if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
        DBG_ERR("Incompatible Driver!!!")
        return result;
    }
    else if (result != VK_SUCCESS) {
        DBG_ERR("Unknown Error")
        return result;
    }

    /*
     * Querying for physical devices
     */
    std::vector<VkPhysicalDevice> devs;
    uint32_t countDevices = 0;

    /* Querying number of physical devices present in the system */
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(vInstance, &countDevices, nullptr)) { 
        DBG_ERR("Cannot find physical devices")
        return result;
    }
    std::cout << "Number of physical devices present: " << countDevices << std::endl;

    /* Populating device array */
    devs.resize(countDevices);
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(vInstance, &countDevices, devs.data())) { 
        DBG_ERR("Cannot query physical devices")
        return result;
    }
    
    for (const auto& dev: devs) { 
        VkPhysicalDeviceProperties devProps = {0};
        vkGetPhysicalDeviceProperties(dev, &devProps);
        std::cout << "Device Type: " << devProps.deviceType << std::endl;
        std::cout << "Device name: " << devProps.deviceName << std::endl;
        /* Querying family queues */
        uint32_t count = 0;
        std::vector<VkQueueFamilyProperties> queueProperties;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
        queueProperties.resize(count);
        std::cout << "Number of queues available in device \"" 
            << devProps.deviceName << "\": " << count << std::endl;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, queueProperties.data());
        for (const auto& queueProp: queueProperties) {
            std::cout << "Queue family: " << queueProp.queueFlags << std::endl;
        }
    }

    DBG_CHECKPOINT;

    return 0;
}
