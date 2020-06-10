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

class Material : public Component
{
public:
    Material(const std::string& name) : Component(name) {}

    Material(Material&& other) = default;

    virtual ~Material() = default;

    virtual std::type_index getType() override { return typeid(Material); }

    std::unordered_map<std::string, Texture*> textures;    

    glm::vec3 emissive{ 0.0f, 0.0f, 0.0f };

    bool      doubleSided{ false };

    float     alphaCutoff{ 0.5f };

    AlphaMode alphaMode{ AlphaMode::Opaque };

}; // class Material

} // namespace sg
} // namespace vkb