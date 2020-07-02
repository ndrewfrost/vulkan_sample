/*
 *
 * Andrew Frost
 * example-vulkan.cpp
 * 2020
 *
 */

#include "example_vulkan.hpp"

namespace vkb {

///////////////////////////////////////////////////////////////////////////
// Example Application                                                   //
///////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
// Prepare the vulkan example
//
void VkExample::setupVulkan(const core::ContextCreateInfo& info, GLFWwindow* window)
{
    // Vk Backend
    core::VkBackend::setupVulkan(info, window);

    // Setup Camera
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    CameraView.setWindowSize(width, height);
    CameraView.setLookAt(glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    //loadAssets()

    //prepareInstanceData()

    //prepareUniformBuffers()

    //setupDescriptorSetLayout()

    //preparePipelines()

    //setupDescriptorPool()

    //setupDescriptorSet()

    //BuildCommandBuffers()
}

//-------------------------------------------------------------------------
// Called on window resize
//
void VkExample::onWindowResize(uint32_t width, uint32_t height)
{
}

} // namespace app