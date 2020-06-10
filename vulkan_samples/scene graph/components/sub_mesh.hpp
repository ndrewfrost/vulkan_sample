/*
 *
 * Andrew Frost
 * sub_mesh.hpp
 * 2020
 *
 */

#pragma once

#include <vector>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <memory>

#include "../component.hpp"
#include "material.hpp"

namespace vkb {
namespace sg  {

///////////////////////////////////////////////////////////////////////////
// SubMesh                                                               //
///////////////////////////////////////////////////////////////////////////

class SubMesh : public Component
{
public:
    virtual ~SubMesh() = default;

    virtual std::type_index getType() override { return typeid(SubMesh); }

    void setMaterial(const Material & material);

    const Material* getMaterial() const { return material; }

    void setAttribute();

    bool getAttribute();

    const ShaderVariant& getShaderVariant() const;

    ShaderVariant& getMutShaderVariant();

private:

    void computeShaderVariant();

public:

    uint32_t indexOffset    = 0;
    uint32_t verticesOffest = 0;
    uint32_t vertexIndices  = 0;

    std::unordered_map<std::string, Buffer> vertexBuffer;

    std::unique_ptr<Buffer> indexBuffer;

private:

    std::unordered_map<std::string, VertexAttribute> vertexAttributes;

    const Material* material{ nullptr };

    ShaderVariant shaderVariant;
};

} // namespace sg
} // namespace vkb