/*
 *
 * Andrew Frost
 * utils.h
 * 2020
 *
 */

#pragma once 

#include <stdexcept>

namespace vkb {

//-------------------------------------------------------------------------
// Get extension of file uri
//
std::string getExtension(const std::string &uri)
{
    auto dotPos = uri.find_last_of('.');

    if (dotPos == std::string::npos)
        throw std::runtime_error("uri has no extension!");

    return uri.substr(dotPos + 1); 
}

} // namespace vkb