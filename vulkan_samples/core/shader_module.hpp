/*
 *
 * Andrew Frost
 * shader_module.hpp
 * 2020
 *
 */

#pragma once

namespace vkb {
namespace core {

///////////////////////////////////////////////////////////////////////////
// ShaderVariant                                                         //
///////////////////////////////////////////////////////////////////////////
// Adds support for c style preprocessor macros to glsl shaders          //
// enabling you to define or undefine certain symbols                    //
///////////////////////////////////////////////////////////////////////////

class ShaderVariant
{
public:
    ShaderVariant() = default;

    void addDefine(const std::string& def);

    void clear();
};

} // namespace core 
} // namespace vkb 