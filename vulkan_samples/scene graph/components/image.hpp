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

namespace vkb {
namespace sg {

struct Mipmap
{
    uint32_t     level  = 0;                     // mipmap level
    uint32_t     offset = 0;                     // byte offset used for uploading
    vk::Extent3D extent = vk::Extent3D{ 0,0,0 }; // Width depth height of mipmap
};

///////////////////////////////////////////////////////////////////////////
// Image                                                                 //
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
    static std::unique_ptr<Image> load(const std::string& name, const std::string& uri)
    {
        std::unique_ptr<Image> image{ nullptr };
        
        // Get file extension
        auto extension = get_extension(uri);

        if (extension == "png" || extension == "jpg")        
            image = std::make_unique<Stb>(name, m_data);        
        else if (extension == "astc")        
            image = std::make_unique<Astc>(name, m_data);        
        else if (extension == "ktx")        
            image = std::make_unique<Ktx>(name, m_data);
        
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
    vk::Format getFormat() const {
        return m_format;
    }

    //---------------------------------------------------------------------
    const vk::Extent3D& getExtent() const { return m_mipmaps.at(0).extent; }

    //---------------------------------------------------------------------
    const uint32_t getLayers() const { return m_layers; }

    //---------------------------------------------------------------------
    const std::vector<Mipmap>& getMipmaps() const { return m_mipmaps; }

    //---------------------------------------------------------------------
    const std::vector<std::vector<VkDeviceSize>>& getOffsets() const { return m_offsets; }

    //---------------------------------------------------------------------
    void generateMipmaps();

    //---------------------------------------------------------------------
    void createVkImage(
        vk::Device&          device,
        vk::ImageViewType    imageViewType = vk::ImageViewType::e2D,
        vk::ImageCreateFlags flags = vk::ImageCreateFlags())
    {
        assert(!m_image && !m_imageView && "Image already constructed");
        m_image = std::make_unique<core::Image>();
        m_imageView = ;
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