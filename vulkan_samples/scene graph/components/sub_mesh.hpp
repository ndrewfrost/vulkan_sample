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
#include "../../core/buffer.hpp"
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

    const Material* getMaterial() const { return m_material; }

    void setAttribute();

    bool getAttribute();

    const ShaderVariant& getShaderVariant() const;

    ShaderVariant& getMutShaderVariant();

private:

    void computeShaderVariant();

public:

    uint32_t m_indexOffset    = 0;
    uint32_t m_verticesOffest = 0;
    uint32_t m_vertexIndices  = 0;

    std::unordered_map<std::string, core::Buffer> m_vertexBuffer;

    std::unique_ptr<core::Buffer> m_indexBuffer;

private:

    std::unordered_map<std::string, VertexAttribute> m_vertexAttributes;

    const Material* m_material{ nullptr };

    ShaderVariant m_shaderVariant;
};

} // namespace sg
} // namespace vkb