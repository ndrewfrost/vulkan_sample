/*
 *
 * Andrew Frost
 * vk_backend.cpp
 * 2020
 *
 */
#define VK_NO_PROTOTYPES
#include "vk_backend.hpp"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace vkb {

///////////////////////////////////////////////////////////////////////////
// VulkanBackend                                                         //
///////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Setup Vulkan Backend
//
void VkBackend::setupVulkan(const ContextCreateInfo& info, GLFWwindow* window)
{
    initInstance(info);

    setupDebugMessenger(false);

    createSurface(window);

    pickPhysicalDevice(info);

    createLogicalDeviceAndQueues(info);

}

//-------------------------------------------------------------------------
// Call on exit
//
void VkBackend::destroy()
{
    m_device.waitIdle();

    m_device.destroy();

    if (m_debugMessenger)
        m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger);

    m_instance.destroySurfaceKHR(m_surface);
    m_instance.destroy();

}

//-------------------------------------------------------------------------
// Create Vulkan Instance
//
void VkBackend::initInstance(const ContextCreateInfo& info)
{
    vk::DynamicLoader         dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
        dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    if (info.enableValidationLayers && !checkValidationLayerSupport(info)) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    vk::ApplicationInfo appInfo = {};
    appInfo.pApplicationName = info.appTitle;
    appInfo.pEngineName      = info.appEngine;
    appInfo.apiVersion       = VK_API_VERSION_1_0;

    vk::InstanceCreateInfo createInfo  = {};
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(info.instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = info.instanceExtensions.data();

    if (info.enableValidationLayers) {
        createInfo.enabledLayerCount   = static_cast<uint32_t>(info.validationLayers.size());
        createInfo.ppEnabledLayerNames = info.validationLayers.data();
    }

    try {
        m_instance = vk::createInstance(createInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create instance!");
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);
}

//-------------------------------------------------------------------------
// Create Surface
//
void VkBackend::createSurface(GLFWwindow* window)
{
    VkSurfaceKHR rawSurface;
    if (glfwCreateWindowSurface(m_instance, window, nullptr, &rawSurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    m_surface = vk::SurfaceKHR(rawSurface);
}

//-------------------------------------------------------------------------
// Pick Physical Device
//
void VkBackend::pickPhysicalDevice(const ContextCreateInfo& info)
{
    std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
    if (devices.size() == 0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");

    // Find a GPU
    for (auto device : devices) {
        uint32_t graphicsIdx = -1;
        uint32_t presentIdx = -1;

        auto queueFamilyProperties = device.getQueueFamilyProperties();
        auto deviceExtensionProperties = device.enumerateDeviceExtensionProperties();

        if (device.getSurfaceFormatsKHR(m_surface).size() == 0) continue;
        if (device.getSurfacePresentModesKHR(m_surface).size() == 0) continue;
        if (!checkDeviceExtensionSupport(info, deviceExtensionProperties))
            continue;

        // supports graphics and compute?
        for (uint32_t j = 0; j < queueFamilyProperties.size(); ++j) {
            vk::QueueFamilyProperties& queueFamily = queueFamilyProperties[j];

            if (queueFamily.queueCount == 0) continue;

            if (queueFamily.queueFlags
                & (vk::QueueFlagBits::eGraphics
                    | vk::QueueFlagBits::eCompute
                    | vk::QueueFlagBits::eTransfer)) {
                graphicsIdx = j;
                break;
            }
        }

        // present queue 
        for (uint32_t j = 0; j < queueFamilyProperties.size(); ++j) {
            vk::QueueFamilyProperties& queueFamily = queueFamilyProperties[j];

            if (queueFamily.queueCount == 0) continue;

            vk::Bool32 supportsPresent = VK_FALSE;
            supportsPresent = device.getSurfaceSupportKHR(j, m_surface);
            if (supportsPresent) {
                presentIdx = j;
                break;
            }
        }

        if (graphicsIdx >= 0 && presentIdx >= 0) {
            m_physicalDevice = device;
            m_graphicsQueueIdx = graphicsIdx;
            m_presentQueueIdx = presentIdx;
            return;
        }
    }

    // Could not find suitable device
    if (!m_physicalDevice) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

//-------------------------------------------------------------------------
// Create Vulkan Device and Queues
//
void VkBackend::createLogicalDeviceAndQueues(const ContextCreateInfo& info)
{
    auto queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { m_graphicsQueueIdx,  m_presentQueueIdx };

    const float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueInfo = {};
        queueInfo.queueFamilyIndex = queueFamily;
        queueInfo.queueCount       = 1;
        queueInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueInfo);
    }
    vk::PhysicalDeviceDescriptorIndexingFeaturesEXT indexFeature = {};

    vk::PhysicalDeviceScalarBlockLayoutFeaturesEXT  scalarFeature = {};
    scalarFeature.pNext = &indexFeature;

    // Vulkan >= 1.1 uses pNext to enable features, and not pEnabledFeatures
    vk::PhysicalDeviceFeatures2 enabledFeatures2 = {};
    enabledFeatures2.features                   = m_physicalDevice.getFeatures();
    enabledFeatures2.features.samplerAnisotropy = VK_TRUE;
    enabledFeatures2.pNext                      = &scalarFeature;
    m_physicalDevice.getFeatures2(&enabledFeatures2);

    vk::DeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos       = queueCreateInfos.data();
    deviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(info.deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = info.deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures        = nullptr;
    deviceCreateInfo.pNext                   = &enabledFeatures2;

    if (info.enableValidationLayers) {
        deviceCreateInfo.enabledLayerCount   = static_cast<uint32_t>(info.validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = info.validationLayers.data();
    }

    try {
        m_device = m_physicalDevice.createDevice(deviceCreateInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create logical device!");
    }

    // Initialize function pointers
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device);

    // Initialize default queues
    m_graphicsQueue = m_device.getQueue(m_graphicsQueueIdx, 0);
    m_presentQueue = m_device.getQueue(m_presentQueueIdx, 0);

    // Initialize debugging tool for queue object names
#if _DEBUG
    m_device.setDebugUtilsObjectNameEXT(
        { vk::ObjectType::eQueue, (uint64_t)(VkQueue)m_graphicsQueue, "graphicsQueue" });

    m_device.setDebugUtilsObjectNameEXT(
        { vk::ObjectType::eQueue, (uint64_t)(VkQueue)m_presentQueue, "presentQueue" });
#endif
}

///////////////////////////////////////////////////////////////////////////
// Debug System Tools                                                    //
///////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// 
//
static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

//-------------------------------------------------------------------------
// Set up Debug Messenger
//
void VkBackend::setupDebugMessenger(bool enableValidationLayers)
{
    if (!enableValidationLayers) return;

    vk::DebugUtilsMessengerCreateInfoEXT debugInfo = {};
    debugInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                              | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
                              | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
    debugInfo.messageType     = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                              | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                              | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
    debugInfo.pfnUserCallback = debugCallback;

    try {
        m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

//-------------------------------------------------------------------------
// check Validation Layer Support
//
bool VkBackend::checkValidationLayerSupport(const ContextCreateInfo& info)
{
    std::vector<vk::LayerProperties> availableLayers =
        vk::enumerateInstanceLayerProperties();

    for (const char* layerName : info.validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------
// check Device Extension Support
//
bool VkBackend::checkDeviceExtensionSupport(const ContextCreateInfo& info, std::vector<vk::ExtensionProperties>& extensionProperties)
{
    std::set<std::string> requiredExtensions(info.deviceExtensions.begin(), 
                                             info.deviceExtensions.end());

    for (const auto& extension : extensionProperties) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

///////////////////////////////////////////////////////////////////////////
// ContextCreateInfo                                                     //
///////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// 
//
ContextCreateInfo::ContextCreateInfo()
{
    deviceExtensions   = std::vector<const char*>();
    validationLayers   = std::vector<const char*>();
    instanceExtensions = std::vector<const char*>();

    if (enableValidationLayers) {
        validationLayers.emplace_back("VK_LAYER_KHRONOS_validation");

        instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}

//-------------------------------------------------------------------------
// 
//
void ContextCreateInfo::addDeviceExtension(const char* name)
{
    deviceExtensions.emplace_back(name);
}


//-------------------------------------------------------------------------
// 
//
void ContextCreateInfo::addInstanceExtension(const char* name)
{
    instanceExtensions.emplace_back(name);
}

//-------------------------------------------------------------------------
// 
//
void ContextCreateInfo::addValidationLayer(const char* name)
{
    deviceExtensions.emplace_back(name);
}

} // namespace vkb