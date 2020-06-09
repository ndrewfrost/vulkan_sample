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

    createSwapChain();

    createCommandPool();

    createCommandBuffer();

    createDepthBuffer();

    createRenderPass();

    createPipelineCache();

    createFrameBuffers();

    createSyncObjects();
}

//-------------------------------------------------------------------------
// Call on exit
//
void VkBackend::destroy()
{
    m_device.waitIdle();

    m_device.destroyRenderPass(m_renderPass);

    m_device.destroyImageView(m_depthView);
    m_device.destroyImage(m_depthImage);
    m_device.freeMemory(m_depthMemory);

    m_device.destroyPipelineCache(m_pipelineCache);

    for (uint32_t i = 0; i < m_swapchain.getImageCount(); i++) {
        m_device.destroyFramebuffer(m_framebuffers[i]);
        m_device.destroyFence(m_fences[i]);
        m_device.freeCommandBuffers(m_commandPool, m_commandBuffers[i]);
    }

    m_swapchain.destroy();

    m_device.destroyCommandPool(m_commandPool);

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
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    m_size = vk::Extent2D(width, height);

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
            
            m_depthFormat = vk::Format::eD32SfloatS8Uint;
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

//-------------------------------------------------------------------------
// create SwapChain
//
void VkBackend::createSwapChain()
{
    m_swapchain.init(m_instance, m_device, m_physicalDevice, m_graphicsQueue, m_graphicsQueueIdx,
        m_presentQueue, m_presentQueueIdx, m_surface, vk::Format::eB8G8R8A8Unorm);

    m_swapchain.update(m_size.width, m_size.height, false);

    m_colorFormat = m_swapchain.getFormat();
}

//-------------------------------------------------------------------------
// Create CommandPool
//
void VkBackend::createCommandPool()
{
    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = m_graphicsQueueIdx;

    try {
        m_commandPool = m_device.createCommandPool(poolInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create command pool!");
    }
}

//-------------------------------------------------------------------------
// Create CommandBuffer
//
void VkBackend::createCommandBuffer()
{
    m_commandBuffers.resize(m_swapchain.getImageCount());

    vk::CommandBufferAllocateInfo cmdBufferAllocInfo = {};
    cmdBufferAllocInfo.commandPool        = m_commandPool;
    cmdBufferAllocInfo.level              = vk::CommandBufferLevel::ePrimary;
    cmdBufferAllocInfo.commandBufferCount = m_swapchain.getImageCount();

    try {
        m_commandBuffers = m_device.allocateCommandBuffers(cmdBufferAllocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

#if _DEBUG
    for (size_t i = 0; i < m_commandBuffers.size(); i++) {
        std::string name = std::string("CmdBufferBackend") + std::to_string(i);
        m_device.setDebugUtilsObjectNameEXT(
            { vk::ObjectType::eCommandBuffer,
            reinterpret_cast<const uint64_t&>(m_commandBuffers[i]), name.c_str() });
    }
#endif
}

//-------------------------------------------------------------------------
// Create Depth Buffer
//
void VkBackend::createDepthBuffer()
{
    // Depth Info
    vk::ImageCreateInfo depthStencilCreateInfo = {};
    depthStencilCreateInfo.imageType   = vk::ImageType::e2D;
    depthStencilCreateInfo.extent      = vk::Extent3D(m_size.width, m_size.height, 1);
    depthStencilCreateInfo.format      = m_depthFormat;
    depthStencilCreateInfo.mipLevels   = 1;
    depthStencilCreateInfo.arrayLayers = 1;
    depthStencilCreateInfo.samples     = vk::SampleCountFlagBits::e1;
    depthStencilCreateInfo.usage       = vk::ImageUsageFlagBits::eDepthStencilAttachment
                                       | vk::ImageUsageFlagBits::eTransferSrc;

    try {
        m_depthImage = m_device.createImage(depthStencilCreateInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create depth images!");
    }

    // find memory requirements
    const vk::MemoryRequirements memReqs = m_device.getImageMemoryRequirements(m_depthImage);
    uint32_t memoryTypeIdx = -1;
    {
        auto deviceMemoryProperties = m_physicalDevice.getMemoryProperties();
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & (1 << i))
                && (deviceMemoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal) {
                memoryTypeIdx = i;
                break;
            }
        }
        if (memoryTypeIdx == -1)
            throw std::runtime_error("failed to find suitable memory type!");
    }

    //Allocate the memory
    vk::MemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.allocationSize = memReqs.size;
    memAllocInfo.memoryTypeIndex = memoryTypeIdx;

    try {
        m_depthMemory = m_device.allocateMemory(memAllocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to allocate depth image memory!");
    }

    // Bind image & memory
    m_device.bindImageMemory(m_depthImage, m_depthMemory, 0);

    // Create an image barrier to change the layout from
    // undefined to DepthStencilAttachmentOptimal

    // barrier on top, barrier inside set up cmdbuffer

    // Depth Info
    const vk::ImageAspectFlags aspect =
        vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

    // Setting up the view
    vk::ImageViewCreateInfo depthStencilView = {};
    depthStencilView.viewType         = vk::ImageViewType::e2D;
    depthStencilView.format           = m_depthFormat;
    depthStencilView.subresourceRange = { aspect, 0, 1, 0, 1 };
    depthStencilView.image            = m_depthImage;
    try {
        m_depthView = m_device.createImageView(depthStencilView);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create depth image view!");
    }
}

//-------------------------------------------------------------------------
// Create RenderPass
//
void VkBackend::createRenderPass()
{
    std::array<vk::AttachmentDescription, 2> attachments = {};
    // Color Attachment
    attachments[0].format         = m_colorFormat;
    attachments[0].samples        = vk::SampleCountFlagBits::e1;
    attachments[0].loadOp         = vk::AttachmentLoadOp::eClear;
    attachments[0].storeOp        = vk::AttachmentStoreOp::eStore;
    attachments[0].stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachments[0].initialLayout  = vk::ImageLayout::eUndefined;
    attachments[0].finalLayout    = vk::ImageLayout::ePresentSrcKHR;
    // Depth Attachment
    attachments[1].format         = m_depthFormat;
    attachments[1].samples        = vk::SampleCountFlagBits::e1;
    attachments[1].loadOp         = vk::AttachmentLoadOp::eClear;
    attachments[1].storeOp        = vk::AttachmentStoreOp::eStore;
    attachments[1].stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    attachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachments[1].initialLayout  = vk::ImageLayout::eUndefined;
    attachments[1].finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    const vk::AttachmentReference colorReference{ 0,  vk::ImageLayout::eColorAttachmentOptimal };
    const vk::AttachmentReference depthReference{ 1, vk::ImageLayout::eDepthStencilAttachmentOptimal };
    
    vk::SubpassDescription subpass  = {};
    subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorReference;
    subpass.pDepthStencilAttachment = &depthReference;

    std::array<vk::SubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass      = 0;
    dependencies[0].srcStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe;
    dependencies[0].dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies[0].srcAccessMask   = vk::AccessFlagBits::eMemoryRead;
    dependencies[0].dstAccessMask   = vk::AccessFlagBits::eColorAttachmentRead
                                    | vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass      = 0;
    dependencies[0].srcStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies[0].dstStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe;
    dependencies[0].srcAccessMask   = vk::AccessFlagBits::eColorAttachmentRead
                                    | vk::AccessFlagBits::eColorAttachmentWrite; 
    dependencies[0].dstAccessMask   = vk::AccessFlagBits::eMemoryRead;
    dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;    

    vk::RenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
    renderPassInfo.pAttachments    = attachments.data();
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());;
    renderPassInfo.pDependencies   = dependencies.data();

    try {
        m_renderPass = m_device.createRenderPass(renderPassInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create render pass!");
    }

#ifdef _DEBUG
    m_device.setDebugUtilsObjectNameEXT(
        { vk::ObjectType::eRenderPass, reinterpret_cast<const uint64_t&>(m_renderPass), "renderPassBackend" });
#endif  

}

//-------------------------------------------------------------------------
// Create PipelineCache
//
void VkBackend::createPipelineCache()
{
    try {
        m_pipelineCache = m_device.createPipelineCache(vk::PipelineCacheCreateInfo());
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create pipeline cache!");
    }
}

//-------------------------------------------------------------------------
// Create FrameBuffers
//
void VkBackend::createFrameBuffers()
{
    std::array<vk::ImageView, 2> attachments;
    attachments[1] = m_depthView;

    vk::FramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.renderPass      = m_renderPass;
    framebufferInfo.attachmentCount = 2;
    framebufferInfo.pAttachments    = attachments.data();
    framebufferInfo.width           = m_size.width;
    framebufferInfo.height          = m_size.height;
    framebufferInfo.layers          = 1;

    // create frame buffer for every swapchain image
    for (uint32_t i = 0; i < m_swapchain.getImageCount(); i++) {        
        attachments[0] = m_swapchain.getImageView(i);      

        try {
            m_framebuffers[i] = m_device.createFramebuffer(framebufferInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

#ifdef _DEBUG
    for (size_t i = 0; i < m_framebuffers.size(); i++) {
        std::string name = std::string("frameBufBack") + std::to_string(i);
        m_device.setDebugUtilsObjectNameEXT(
            { vk::ObjectType::eFramebuffer, reinterpret_cast<const uint64_t&>(m_framebuffers[i]), name.c_str() });
    }
#endif
}

//-------------------------------------------------------------------------
// Create SynchObjects
//
void VkBackend::createSyncObjects()
{
    m_fences.resize(m_swapchain.getImageCount());

    try {
        for (uint32_t i = 0; i < m_swapchain.getImageCount(); ++i) {
            m_fences[i] = m_device.createFence({ vk::FenceCreateFlagBits::eSignaled });
        }
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

//-------------------------------------------------------------------------
// function to call before rendering
//
void VkBackend::prepareFrame()
{// Acquire the next image from the swap chain
    auto result = m_swapchain.acquire();
    // Recreate the swapchain if it's no longer compatible with the surface
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        onWindowResize(m_size.width, m_size.height);
    }
    else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to acquire image from swapchain!");
    }

    // fence until cmd buffer has finished executing before using again
    uint32_t imageIndex = m_swapchain.getActiveImageIndex();
    while (m_device.waitForFences(m_fences[imageIndex], VK_TRUE, 10000) == vk::Result::eTimeout) {}
}

//-------------------------------------------------------------------------
// function to call for submitting the rendering command
//
void VkBackend::submitFrame()
{
    uint32_t imageIndex = m_swapchain.getActiveImageIndex();
    m_device.resetFences(m_fences[imageIndex]);

    vk::Semaphore semaphoreRead  = m_swapchain.getActiveReadSemaphore();
    vk::Semaphore semaphoreWrite = m_swapchain.getActiveWrittenSemaphore();

    // Pipeline stage at which the queue submission will wait (via pWaitSemaphores)
    const vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo submitInfo = {};
    submitInfo.waitSemaphoreCount   = 1;                              // One wait semaphore
    submitInfo.pWaitSemaphores      = &semaphoreRead;                 // Semaphore(s) to wait upon before the submitted command buffer starts executing
    submitInfo.pWaitDstStageMask    = &waitStageMask;                 // Pointer to the list of pipeline stages that the semaphore waits will occur at
    submitInfo.commandBufferCount   = 1;                              // One Command Buffer
    submitInfo.pCommandBuffers      = &m_commandBuffers[imageIndex];  // Command buffers(s) to execute in this batch (submission)
    submitInfo.signalSemaphoreCount = 1;                              // One signal Semaphore
    submitInfo.pSignalSemaphores    = &semaphoreWrite;                // Semaphore(s) to be signaled when command buffers have completed

    // Submit to the graphics queue passing a wait fence
    try {
        m_graphicsQueue.submit(submitInfo, m_fences[imageIndex]);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    m_swapchain.present(m_graphicsQueue);
}

//-------------------------------------------------------------------------
// On Window Size Callback
// - Destroy allocated frames, then rebuild with new size
//
void VkBackend::onWindowResize(uint32_t width, uint32_t height)
{
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