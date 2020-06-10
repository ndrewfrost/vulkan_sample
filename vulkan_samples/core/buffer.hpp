/*
 *
 * Andrew Frost
 * buffer.hpp
 * 2020
 *
 */

#pragma once

#include <vulkan/vulkan.hpp>
#include "../external/vma/vk_mem_alloc.h"

namespace vkb {
namespace core {

///////////////////////////////////////////////////////////////////////////
// Buffer                                                                //
///////////////////////////////////////////////////////////////////////////

class Buffer
{
public:
    Buffer();

    Buffer(const Buffer&) = delete;

    Buffer(Buffer&& other);

    ~Buffer();

    Buffer& operator=(const Buffer&) = delete;

    Buffer& operator=(Buffer&&) = delete;

private:
    vk::Device& device;

    vk::Buffer handle = nullptr;

    VmaAllocation memory{ VK_NULL_HANDLE };

};

} // namespace core
} // namespace vkb
