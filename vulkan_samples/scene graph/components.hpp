/*
 *
 * Andrew Frost
 * components.hpp
 * 2020
 *
 */

#pragma once

#include <string>
#include <algorithm>

namespace vkb {
namespace sg {

///////////////////////////////////////////////////////////////////////////
// Component                                                             //
///////////////////////////////////////////////////////////////////////////

class Component {
public:
    Component() = default;

    Component(const std::string& name) : name(name) {}

    Component(Component&& other) = default;

    virtual ~Component() = default;

    const std::string& get_name() const { return name; }

    virtual std::type_index get_type() = 0;

private:
    std::string name;
};

} // namespace sg
} // namespace vkb