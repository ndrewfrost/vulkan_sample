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
// A Generic class which can be used by nodes                            //
///////////////////////////////////////////////////////////////////////////

class Component
{
public:
    //---------------------------------------------------------------------
    // Contructor
    //
    Component() = default;

    //---------------------------------------------------------------------
    // Assign name
    //
    Component(const std::string& name) : m_name(name) {}

    //---------------------------------------------------------------------
    // Move constructor
    //
    Component(Component&& other) = default;

    //---------------------------------------------------------------------
    // ~
    //
    virtual ~Component() = default;

    //---------------------------------------------------------------------
    // Get Component Name
    //
    const std::string& getName() const { return m_name; }

    //---------------------------------------------------------------------
    // get Type of Component
    //
    virtual std::type_index getType() = 0;

private:
    std::string m_name;
};

} // namespace sg
} // namespace vkb