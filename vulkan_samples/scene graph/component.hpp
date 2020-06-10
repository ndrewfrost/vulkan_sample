/*
 *
 * Andrew Frost
 * component.hpp
 * 2020
 *
 */

#pragma once

#include <string>
#include <typeindex>

namespace vkb {
namespace sg {

///////////////////////////////////////////////////////////////////////////
// Component                                                             //
///////////////////////////////////////////////////////////////////////////

class Component
{
public:
    Component() = default;

    Component(const std::string& name) : name(name) {}

    Component(Component&& other) = default;

    virtual ~Component() = default;

    const std::string& getName() const { return name; }

    virtual std::type_index getType() = 0;

private:
    std::string name;
};

} // namespace sg
} // namespace vkb