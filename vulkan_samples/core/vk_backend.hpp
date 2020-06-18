/*
 *
 * Andrew Frost
 * vk_backend.hpp
 * 2020
 *
 */

#pragma once

#include <set>
#include <iostream>
#include <vulkan/vulkan.hpp>

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "swapchain.hpp"

namespace vkb {
namespace core {

///////////////////////////////////////////////////////////////////////////
// ContextCreateInfo                                                     //
///////////////////////////////////////////////////////////////////////////
// Allows app to specify features that are expected of                   //
// - vk::Instance                                                        //
// - vk::Device                                                          //
///////////////////////////////////////////////////////////////////////////
struct ContextCreateInfo
{
    ContextCreateInfo();

    void addDeviceExtension(const char* name);

    void addInstanceExtension(const char* name);

    void addValidationLayer(const char* name);

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    std::vector<const char*> deviceExtensions;

    std::vector<const char*> validationLayers;

    std::vector<const char*> instanceExtensions;

    const char* appEngine = "No Engine";
    const char* appTitle = "Application";
};

///////////////////////////////////////////////////////////////////////////
// VulkanBackend                                                         //
///////////////////////////////////////////////////////////////////////////

class VkBackend
{
public:

    VkBackend() = default;

    virtual ~VkBackend() = default;

    virtual void setupVulkan(const ContextCreateInfo& info, GLFWwindow* window);

    void destroy();

    void initInstance(const ContextCreateInfo& info);

    void createSurface(GLFWwindow* window);

    void pickPhysicalDevice(const ContextCreateInfo& info);

    void createLogicalDeviceAndQueues(const ContextCreateInfo& info);

    void createSwapChain();

    void createCommandPool();

    void createCommandBuffer();

    void createRenderPass();

    void createPipelineCache();

    void createDepthBuffer();

    void createFrameBuffers();

    void createSyncObjects();

    void prepareFrame();

    void submitFrame();

    virtual void onWindowResize(uint32_t width, uint32_t height);

    ///////////////////////////////////////////////////////////////////////////
    // Debug System Tools                                                    //
    ///////////////////////////////////////////////////////////////////////////

    void setupDebugMessenger(bool enabelValidationLayers);

    bool checkValidationLayerSupport(const ContextCreateInfo& info);

    bool checkDeviceExtensionSupport(const ContextCreateInfo& info, std::vector<vk::ExtensionProperties>& extensionProperties);

    VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;

    //-------------------------------------------------------------------------
    // Collection of Getter Methods
    //
    vk::Instance                          getInstance() { return m_instance; }
    vk::Device                            getDevice() { return m_device; }
    vk::PhysicalDevice                    getPhysicalDevice() { return m_physicalDevice; }
    vk::SurfaceKHR                        getSurface() { return m_surface; }
    vk::Queue                             getGraphicsQueue() { return m_graphicsQueue; }
    uint32_t                              getGraphicsQueueIdx() { return m_graphicsQueueIdx; }
    vk::Queue                             getPresentQueue() { return m_presentQueue; }
    uint32_t                              getPresentQueueIdx() { return m_presentQueueIdx; }
    vk::Extent2D                          getSize() { return m_size; }
    vk::RenderPass                        getRenderPass() { return m_renderPass; }
    vk::PipelineCache                     getPipelineCache() { return m_pipelineCache; }
    const std::vector<vk::Framebuffer>&   getFramebuffers() { return m_framebuffers; }
    const std::vector<vk::CommandBuffer>& getCommandBuffers() { return m_commandBuffers; }
    uint32_t                              getCurrentFrame() const { return m_swapchain.getActiveImageIndex(); }
    vk::Format                            getColorFormat()  const { return m_colorFormat; }
    vk::Format                            getDepthFormat()  const { return m_depthFormat; }
    vk::SampleCountFlagBits               getSampleCount()  const { return m_sampleCount; }

protected:

    vk::Instance                   m_instance;
    vk::PhysicalDevice             m_physicalDevice;
    vk::Device                     m_device;

    vk::SurfaceKHR                 m_surface;

    vk::Queue                      m_graphicsQueue;
    vk::Queue                      m_presentQueue;
    uint32_t                       m_graphicsQueueIdx{ VK_QUEUE_FAMILY_IGNORED };
    uint32_t                       m_presentQueueIdx{ VK_QUEUE_FAMILY_IGNORED };

    vkb::core::SwapChain           m_swapchain;
    std::vector<vk::Framebuffer>   m_framebuffers;
    std::vector<vk::CommandBuffer> m_commandBuffers;

    vk::CommandPool                m_commandPool;

    vk::Image                      m_depthImage;
    vk::DeviceMemory               m_depthMemory;
    vk::ImageView                  m_depthView;

    vk::RenderPass                 m_renderPass;
    vk::PipelineCache              m_pipelineCache;

    std::vector<vk::Fence>         m_fences;

    vk::Extent2D                   m_size{ 0, 0 };

    vk::Format                     m_depthFormat{ vk::Format::eUndefined };
    vk::Format                     m_colorFormat{ vk::Format::eUndefined };
    vk::SampleCountFlagBits        m_sampleCount{ vk::SampleCountFlagBits::e1 };

}; // classVkBackend

} // namespace core 
} // namespace vkb