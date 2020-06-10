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
#include "scene graph/components/sub_mesh.hpp"

namespace vkb {

///////////////////////////////////////////////////////////////////////////
// VulkanExample                                                         //
///////////////////////////////////////////////////////////////////////////

class VkExample : public core::VkBackend
{
public:

    VkExample() = default;

    virtual ~VkExample();

    virtual void setupVulkan(const core::ContextCreateInfo& info, GLFWwindow* window) override;

    void loadAssets();

    void createDescriptorSetLayout();

    void createPipelines();

    void createUniformBuffer();

    void updateUniformBuffer();

    void render();

    virtual void onWindowResize(uint32_t width, uint32_t height) override;
    
protected:


}; // Class VkExample

}  // namespace app