/*
 *
 * Andrew Frost
 * example_vulkan.hpp
 * 2020
 *
 */

#pragma once

#include <vulkan/vulkan.hpp>
#include "core/vk_backend.hpp"

namespace vkb {

///////////////////////////////////////////////////////////////////////////
// VulkanExample                                                         //
///////////////////////////////////////////////////////////////////////////

class VkExample : public core::VkBackend
{
public:

    VkExample() = default;

    virtual ~VkExample() = default;

    virtual void setupVulkan(const core::ContextCreateInfo& info, GLFWwindow* window) override;

    void run();

    virtual void onWindowResize(uint32_t width, uint32_t height) override;
    
protected:


}; // Class VkExample

}  // namespace app