/*
 *
 * Andrew Frost
 * stb.hpp
 * 2020
 *
 */

#pragma once

#include "../image.hpp"

namespace vkb {
namespace sg {

///////////////////////////////////////////////////////////////////////////
// Stb                                                                   //
///////////////////////////////////////////////////////////////////////////

class Stb : public Image
{
public:
    //---------------------------------------------------------------------
    Stb(const std::string& name, const std::vector<uint8_t>& data);

    //---------------------------------------------------------------------
    virtual ~Stb() = default;
};

} // namespace sg
} // namespace vkb
