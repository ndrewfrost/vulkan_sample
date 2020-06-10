/*
 *
 * Andrew Frost
 * material.hpp
 * 2020
 *
 */

#pragma once

#include "../component.hpp"
#include "texture.hpp"
#include "../../common/glm_common.h"

namespace vkb {
namespace sg {

//-------------------------------------------------------------------------
// How the alpha value should be interpreted
//
enum class AlphaMode {
    Opaque, Mask, Blend
};

///////////////////////////////////////////////////////////////////////////
// Material                                                              //
///////////////////////////////////////////////////////////////////////////
// Material associated with a submesh. A collection of textures and      //
// alpha treatment                                                       //
///////////////////////////////////////////////////////////////////////////

class Material : public Component
{
public:
    //---------------------------------------------------------------------
    //
    //
    Material(const std::string& name) : Component(name) {}

    //---------------------------------------------------------------------
    // 
    //
    Material(Material&& other) = default;

    //---------------------------------------------------------------------
    // 
    //
    virtual ~Material() = default;

    //---------------------------------------------------------------------
    // 
    //
    virtual std::type_index getType() override { return typeid(Material); }

public:
    std::unordered_map<std::string, Texture*> m_textures;

    glm::vec3 m_emissive{ 0.0f, 0.0f, 0.0f };

    bool      m_doubleSided{ false };

    float     m_alphaCutoff{ 0.5f };

    AlphaMode m_alphaMode{ AlphaMode::Opaque };

}; // class Material

} // namespace sg
} // namespace vkb