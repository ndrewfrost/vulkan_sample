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
    //VkBackend
    core::VkBackend::setupVulkan(info, window);

    //setUpCamera()

    //loadAssets()

    //prepareInstanceData()

    //prepareUniformBuffers()

    //setupDescriptorSetLayout()

    //preparePipelines()

    //setupDescriptorPool()

    //setupDescriptorSet()

    //BuildCommandBuffers()
}

} // namespace app