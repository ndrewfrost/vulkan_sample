/*
 *
 * Andrew Frost
 * image.hpp
 * 2020
 *
 */

#pragma once

#include <vector>

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
private:
protected:
private:
    std::vector<uint8_t> m_data;

    vk::Format           m_format{ vk::Format::eUndefined };

    std::vector<Mipmap>  m_mipmaps{ {} };


};

} // namespace sg
} // namespace vkb