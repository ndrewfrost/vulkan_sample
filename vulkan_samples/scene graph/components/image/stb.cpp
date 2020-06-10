
#include "stb.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vkb {
namespace sg {

///////////////////////////////////////////////////////////////////////////
// Stb                                                                   //
///////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------
Stb::Stb(const std::string& name, const std::vector<uint8_t>& data) : Image(name)
{
    int width, height, comp, reqComp = 4;

    auto dataBuffer = reinterpret_cast<const stbi_uc*>(data.data());
    auto dataSize = static_cast<int>(data.size());

    auto rawData = stbi_load_from_memory(dataBuffer, dataSize, &width, &height, &comp, reqComp);

    if (!rawData)
        throw std::runtime_error{ "Failed to load " + name + ": " + stbi_failure_reason() };

    setData(rawData, width * height * reqComp);
    stbi_image_free(rawData);

    setFormat(vk::Format::eR8G8B8A8Unorm);
    setWidth(static_cast<uint32_t>(width));
    setHeight(static_cast<uint32_t>(height));
    setDepth(1u);
}

} // namespace sg
} // namespace vkb
