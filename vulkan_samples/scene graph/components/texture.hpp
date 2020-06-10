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

class Texture : public Component
{
public:
    Texture(const std::string& name) : Component(name) {}

    Texture(Texture&& other) = default;

    virtual ~Texture() = default;

    virtual std::type_index getType() override { return typeid(Texture); }

    void setImage(Image& image);

    Image* getImage();

    void setSampler(Sampler& sampler);

    Sampler* getSampler();

private:
    Image*   image{ nullptr };
    Sampler* sampler{ nullptr };

}; // class Texture

} // namespace sg
} // namespace vkb