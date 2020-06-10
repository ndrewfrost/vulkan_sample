/*
 *
 * Andrew Frost
 * image.hpp
 * 2020
 *
 */

#pragma once

#include <vector>
#include "../../core/image.hpp"
#include "../component.hpp"
#include "image/ktx.hpp"
#include "image/astc.hpp"
#include "image/stb.hpp"
#include "../../common/utils.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

namespace vkb {
namespace sg {

//-------------------------------------------------------------------------
// Mimmap information
//
struct Mipmap
{
    uint32_t     level  = 0;                     // mipmap level
    uint32_t     offset = 0;                     // byte offset used for uploading
    vk::Extent3D extent = vk::Extent3D{ 0,0,0 }; // Width depth height of mipmap
};

///////////////////////////////////////////////////////////////////////////
// Image                                                                 //
///////////////////////////////////////////////////////////////////////////
// Image associated with texture for scene graph                         //
///////////////////////////////////////////////////////////////////////////

class Image : public Component
{
public:
    //---------------------------------------------------------------------
    Image(const std::string& name,
        std::vector<uint8_t>&& data = {},
        std::vector<Mipmap>&& mipmaps = { {} }) 
        : Component(name)
    {
        m_data = std::move(data);
        m_format = vk::Format::eR8G8B8A8Unorm;
        m_mipmaps = std::move(mipmaps);
    }

    //---------------------------------------------------------------------
    // Load image, checking for file extension
    //
    std::unique_ptr<Image> load(const std::string& name, const std::string& uri)
    {
        std::unique_ptr<Image> image{ nullptr };
        
        //std::vector<uint8_t> data = fs::read_asset(uri);

        // Get file extension
        auto extension = getExtension(uri);

        if (extension == "png" || extension == "jpg")        
            image = std::make_unique<Stb>(name, data);        
        else if (extension == "astc")        
            image = std::make_unique<Astc>(name, data);        
        else if (extension == "ktx")        
            image = std::make_unique<Ktx>(name, data);
        
        return image;
    }

    //---------------------------------------------------------------------
    virtual ~Image() = default;

    //---------------------------------------------------------------------
    virtual std::type_index getType() override { return typeid(Image); }

    //---------------------------------------------------------------------
    const std::vector<uint8_t>& getData() const { return m_data; }

    //---------------------------------------------------------------------
    void clearData()
    {
        m_data.clear();
        m_data.shrink_to_fit();
    }

    //---------------------------------------------------------------------
    vk::Format getFormat() const { return m_format; }

    //---------------------------------------------------------------------
    const vk::Extent3D& getExtent() const { return m_mipmaps.at(0).extent; }

    //---------------------------------------------------------------------
    const uint32_t getLayers() const { return m_layers; }

    //---------------------------------------------------------------------
    const std::vector<Mipmap>& getMipmaps() const { return m_mipmaps; }

    //---------------------------------------------------------------------
    const std::vector<std::vector<VkDeviceSize>>& getOffsets() const { return m_offsets; }

    //---------------------------------------------------------------------
    void generateMipmaps()
    {
        assert(m_mipmaps.size() == 1 && "Mipmaps already generated");

        if (m_mipmaps.size() > 1) return; 
        
        vk::Extent3D extent     = getExtent();
        uint32_t     nextWidth  = std::max<uint32_t>(1u, extent.width/2); 
        uint32_t     nextHeight = std::max<uint32_t>(1u, extent.height/2);
        uint32_t     channels   = 4;
        uint32_t     nextSize   = channels * nextHeight * nextWidth;

        while (true) {
            // make space for next mipmap
            uint32_t oldSize = static_cast<uint32_t>(m_data.size());
            m_data.resize(oldSize + nextSize);

            auto& previousMipmap = m_mipmaps.back();
            // update mipmaps
            Mipmap nextMipmap = {};
            nextMipmap.level  = previousMipmap.level + 1;
            nextMipmap.offset = oldSize;
            nextMipmap.extent = vk::Extent3D{nextWidth, nextHeight, 1u};

            // fill next mipmap memory
//THIRD PARTY TODO
            stbir_resize_uint8(m_data.data() + previousMipmap.offset, previousMipmap.extent.width, previousMipmap.extent.height, 0,
                               m_data.data() + nextMipmap.offset, nextMipmap.extent.width, nextMipmap.extent.height, 0, channels);

            m_mipmaps.emplace_back(std::move(nextMipmap));

            // next mipmap values
            nextWidth  = std::max<uint32_t>(1u, nextWidth / 2);
            nextHeight = std::max<uint32_t>(1u, nextHeight / 2);
            nextSize   = nextWidth * nextHeight * channels;

            if (nextWidth == 1 && nextHeight == 1)
            {
                break;
            }
        }
    }

    //---------------------------------------------------------------------
    void createVkImage(
        vk::Device&          device,
        vk::ImageViewType    imageViewType = vk::ImageViewType::e2D,
        vk::ImageCreateFlags flags = vk::ImageCreateFlags())
    {
        assert(!m_image && !m_imageView && "Image already constructed");
       
        // image
        m_image = std::make_unique<core::Image>(device, getExtent(), m_format, 
            vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
            VMA_MEMORY_USAGE_GPU_ONLY, vk::SampleCountFlagBits::e1, m_layers, 
            vk::ImageTiling::eOptimal);
        
        // image view
        vk::ImageViewCreateInfo viewInfo = {};

        switch (imageViewType) {
        case vk::ImageViewType::e1D:
            viewInfo.viewType = vk::ImageViewType::e1D;
            break;
        case vk::ImageViewType::e2D:
            viewInfo.viewType = vk::ImageViewType::e2D;
            break;
        case vk::ImageViewType::e3D:
            viewInfo.viewType = vk::ImageViewType::e3D;
            break;
        default:
            assert(0);
        }

        viewInfo.format = m_format;
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        try {
            m_imageView = device.createImageView(viewInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create ImageView(SG)!");
        }
    }

    //---------------------------------------------------------------------
    const core::Image& getVkImage() const
    {
        assert(m_image && "image was not created");
        return *m_image;
    }

    //---------------------------------------------------------------------
    const vk::ImageView& getVkImageView() const
    {
        assert(m_imageView && "image view was not created");
        return m_imageView;
    }

protected:
    std::vector<uint8_t>& getMutData()   { return m_data; }

    void setData(const uint8_t* raw_data, size_t size)
                                         { assert(m_data.empty() && "image data already set");
                                           m_data = { raw_data, raw_data + size}; }

    void setFormat(vk::Format format)    { m_format = format; }

    void setWidth(uint32_t width)        { m_mipmaps.at(0).extent.width = width; }

    void setHeight(uint32_t height)      { m_mipmaps.at(0).extent.height = height; }

    void setDepth(uint32_t depth)        { m_mipmaps.at(0).extent.depth = depth; }
     
    void setLayers(uint32_t layers)      { m_layers = layers; }
      
    void setOffsets(const std::vector<std::vector<vk::DeviceSize>>& offsets) 
                                         { m_offsets = offsets; }

    Mipmap& getMipmap(size_t index)      { return m_mipmaps.at(index); }

    std::vector<Mipmap>& getMutMipmaps() { return m_mipmaps; }

private:
    std::vector<uint8_t>                     m_data;

    vk::Format                               m_format{ vk::Format::eUndefined };

    uint32_t                                 m_layers{ 1 };

    std::vector<Mipmap>                      m_mipmaps{ {} };
   
    std::vector<std::vector<vk::DeviceSize>> m_offsets;  // Offsets stored like offsets[array_layer][mipmap_layer]

    std::unique_ptr<core::Image>             m_image;

    vk::ImageView                            m_imageView;
};

} // namespace sg
} // namespace vkb