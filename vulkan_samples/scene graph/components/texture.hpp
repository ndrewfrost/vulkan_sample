/*
 *
 * Andrew Frost
 * texture.hpp
 * 2020
 *
 */

#pragma once

#include "image.hpp"
#include "sampler.hpp"
#include "../component.hpp"

namespace vkb {
namespace sg {

///////////////////////////////////////////////////////////////////////////
// Texture                                                               //
///////////////////////////////////////////////////////////////////////////
// An Image with an associated Sampler                                   //
///////////////////////////////////////////////////////////////////////////

class Texture : public Component
{
public:
    //---------------------------------------------------------------------
    Texture(const std::string& name) : Component(name) {}

    //---------------------------------------------------------------------
    Texture(Texture&& other) = default;

    //---------------------------------------------------------------------
    virtual ~Texture() = default;

    //---------------------------------------------------------------------
    virtual std::type_index getType() override { return typeid(Texture); }

    //---------------------------------------------------------------------
    void setImage(Image& image) { m_image = &image; }

    //---------------------------------------------------------------------
    Image* getImage() { return m_image; }

    //---------------------------------------------------------------------
    void setSampler(Sampler& sampler) { m_sampler = &sampler; }

    //---------------------------------------------------------------------
    Sampler* getSampler() { return m_sampler; }

private:
    Image*   m_image{ nullptr };
    Sampler* m_sampler{ nullptr };

}; // class Texture

} // namespace sg
} // namespace vkb