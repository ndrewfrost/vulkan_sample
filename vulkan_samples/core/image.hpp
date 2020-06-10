/*
 *
 * Andrew Frost
 * image.hpp
 * 2020
 *
 */

#pragma once

#include <vulkan/vulkan.hpp>
#include "../external/vma/vk_mem_alloc.h"

namespace vkb {
namespace core {

///////////////////////////////////////////////////////////////////////////
// Image                                                                 //
///////////////////////////////////////////////////////////////////////////

class Image
{
public:
    Image();

    Image(const Image&) = delete;

    Image(Image&& other);

    ~Image();

    Image& operator=(const Image&) = delete;

    Image& operator=(Image&&) = delete;

private:
    vk::Device&   m_device;

    vk::Image     m_handle = nullptr;

    VmaAllocation m_memory{ VK_NULL_HANDLE };

    vk::ImageType m_type;
};

} // namespace core
} // namespace vkb