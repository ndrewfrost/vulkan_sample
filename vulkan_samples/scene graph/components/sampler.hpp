/*
 *
 * Andrew Frost
 * sampler.hpp
 * 2020
 *
 */

#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include "../component.hpp"

namespace vkb {
namespace sg {

///////////////////////////////////////////////////////////////////////////
// Sampler                                                               //
///////////////////////////////////////////////////////////////////////////

class Sampler : public Component
{
public:
    Sampler(const std::string& name, vk::Sampler sampler)
        : Component(name) { m_sampler = sampler; }

    Sampler(Sampler&& other) = default;

    virtual ~Sampler() = default;

    virtual std::type_index getType() override { return typeid(Sampler); }

public:
    vk::Sampler m_sampler;
};

} // namespace sg
} // namespace vkb