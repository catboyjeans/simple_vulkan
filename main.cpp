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

void printPhysicalDeviceFeatures(VkPhysicalDevice& dev) {

}

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
    VkInstance vInstance = { }; 
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
        VkPhysicalDeviceProperties devProps = { };
        vkGetPhysicalDeviceProperties(dev, &devProps);
        std::cout << "Device Type: " << devProps.deviceType << std::endl;
        std::cout << "Device name: " << devProps.deviceName << std::endl;

        /*
         * Querying for Device Features
         */
        VkPhysicalDeviceFeatures devFeatures = { };
        vkGetPhysicalDeviceFeatures(dev, &devFeatures);

        /*
         * Querying for Device Memory types
         */
        VkPhysicalDeviceMemoryProperties memProps = { };
        vkGetPhysicalDeviceMemoryProperties(dev, &memProps);

        /* 
         * Querying queue Properties
         */
        uint32_t count = 0;
        std::vector<VkQueueFamilyProperties> queueProperties;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
        queueProperties.resize(count);
        std::cout << "Number of queues available in device \"" 
            << devProps.deviceName << "\": " << count << std::endl;

        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, queueProperties.data());
        for (const auto& queueProp: queueProperties) {
            std::cout << "Queue family type: " 
                << queueProp.queueFlags << "(" << queueProp.queueCount << ")" << std::endl;
        }
    }

    /*
     * Querying Physical Device groups
     */
    uint32_t devGroupCount = 0;
    std::vector<VkPhysicalDeviceGroupProperties> devGroupProperties;
    vkEnumeratePhysicalDeviceGroups(vInstance, &devGroupCount, nullptr);
    devGroupProperties.resize(devGroupCount);
    std::cout << "There are: " << devGroupCount << " Physical Device Groups" << std::endl;
    vkEnumeratePhysicalDeviceGroups(vInstance, &devGroupCount, devGroupProperties.data());

    for (const auto& devGroup: devGroupProperties) { 
        std::cout << "Group Type: " << devGroup.sType << std::endl;
        std::cout << "Group Devices: " << devGroup.physicalDeviceCount << std::endl;
    }

    /*
     * Creating Logical Device
     */
    VkDeviceQueueCreateInfo devQueue = { 
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = 0,
        .pQueuePriorities = nullptr,
    };

    VkDeviceCreateInfo devInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .pQueueCreateInfos = &devQueue,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr,
        .pEnabledFeatures = nullptr, /* Put here the Physical dev features from vkGetPhysicalDevFeat */
    };

    VkPhysicalDeviceFeatures devFeatures = { };
    vkGetPhysicalDeviceFeatures(devs.front(), &devFeatures);
    devInfo.pEnabledFeatures = &devFeatures;
    
    VkDevice dev = { };
    if (VK_SUCCESS != vkCreateDevice(devs.front(), &devInfo, nullptr, &dev)) {
        DBG_ERR("Bad call to vkCreateDevice");
    }

    /* 
     * Enumerate Instance Layers
     */
    uint32_t instPropCount = 0;
    std::vector<VkLayerProperties> instanceLayerProperties;
    vkEnumerateInstanceLayerProperties(&instPropCount, nullptr);
    instanceLayerProperties.resize(instPropCount);
    vkEnumerateInstanceLayerProperties(&instPropCount, instanceLayerProperties.data());
    std::cout << "-- Querying for instance layer properties: " << std::endl;
    std::cout << "Available layer properties: " << instPropCount << std::endl;
    for (const auto& instanceLayerProperty: instanceLayerProperties) {
        std::cout << instanceLayerProperty.layerName << std::endl;
        std::cout << "\t" << instanceLayerProperty.description << std::endl;
    }

    /* 
     * Enumerate Device Layers
     */
    uint32_t devPropCount = 0;
    std::vector<VkLayerProperties> deviceLayerProperties;
    vkEnumerateDeviceLayerProperties(devs.back(), &devPropCount, nullptr);
    instanceLayerProperties.resize(devPropCount);
    vkEnumerateDeviceLayerProperties(devs.back(), &devPropCount, deviceLayerProperties.data());
    std::cout << "-- Querying for Device layer properties: " << std::endl;
    std::cout << "Available layer properties: " << devPropCount << std::endl;
    for (const auto& deviceLayerProp: deviceLayerProperties) {
        std::cout << deviceLayerProp.layerName << std::endl;
        std::cout << "\t" << deviceLayerProp.description << std::endl;
    }

    DBG_CHECKPOINT;

    return 0;
}
