/*
 *
 * Andrew Frost
 * ktx.hpp
 * 2020
 *
 */

#pragma once

#include "../image.hpp"


namespace vkb {
namespace sg {

///////////////////////////////////////////////////////////////////////////
// Ktx                                                                   //
///////////////////////////////////////////////////////////////////////////

class Ktx : public Image
{
public:
    //---------------------------------------------------------------------
    Ktx(const std::string& name, const std::vector<uint8_t>& data) : Image(name)
    {
        
    }

    //---------------------------------------------------------------------
    virtual ~Ktx() = default;
};

} // namespace sg
} // namespace vkb
