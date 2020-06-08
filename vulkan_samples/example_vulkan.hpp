/*
 *
 * Andrew Frost
 * examplevulkan.hpp
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

class VulkanExample
{
public:

    VulkanExample() = default;

    virtual ~VulkanExample();

    virtual void prepare();

    virtual void update();
    
    virtual void resize();

    virtual void inputEvents();

    virtual void finish();
    
protected:


}; // Class ExampleVulkan

}  // namespace app