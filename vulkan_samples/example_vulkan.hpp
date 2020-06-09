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

class VkExample : public VkBackend
{
public:

    VkExample() = default;

    virtual ~VkExample();

    virtual void prepare();

    virtual void update();
    
    virtual void resize();

    virtual void inputEvents();

    virtual void finish();
    
protected:


}; // Class VkExample

}  // namespace app