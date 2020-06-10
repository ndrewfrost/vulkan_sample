/*
 *
 * Andrew Frost
 * astc.hpp
 * 2020
 *
 */

#pragma once

#include "../image.hpp"


namespace vkb {
namespace sg {

///////////////////////////////////////////////////////////////////////////
// Astc                                                                  //
///////////////////////////////////////////////////////////////////////////

class Astc : public Image
{
public:
    //---------------------------------------------------------------------
    Astc(const std::string& name, const std::vector<uint8_t>& data) : Image(name)
    {

    }

    //---------------------------------------------------------------------
    virtual ~Astc() = default;
};

} // namespace sg
} // namespace vkb