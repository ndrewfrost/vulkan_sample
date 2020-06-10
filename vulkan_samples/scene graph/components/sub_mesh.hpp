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
#include "../../core/shader_module.hpp"
#include "material.hpp"

namespace vkb {
namespace sg  {

//-------------------------------------------------------------------------
// Vertex Attribute
//
struct VertexAttribute
{
    vk::Format format = vk::Format::eUndefined;
    uint32_t   stride = 0;
    uint32_t   offset = 0;
};

///////////////////////////////////////////////////////////////////////////
// SubMesh                                                               //
///////////////////////////////////////////////////////////////////////////

class SubMesh : public Component
{
public:
    //---------------------------------------------------------------------
    virtual ~SubMesh() = default;

    //---------------------------------------------------------------------
    virtual std::type_index getType() override { return typeid(SubMesh); }

    //---------------------------------------------------------------------
    void setMaterial(const Material& material)
    {
        m_material = &material;
        computeShaderVariant();
    }

    //---------------------------------------------------------------------
    const Material* getMaterial() const { return m_material; }

    //---------------------------------------------------------------------
    void setAttribute(const std::string& name, const VertexAttribute& attribute)
    {
        m_vertexAttributes[name] = attribute;
        computeShaderVariant();
    }

    //---------------------------------------------------------------------
    bool getAttribute(const std::string& name, VertexAttribute& attribute)
    {
        auto attributeIter = m_vertexAttributes.find(name);

        if (attributeIter == m_vertexAttributes.end()) 
            return false;
        
        attribute = attributeIter->second;
        return true;
    }

    //---------------------------------------------------------------------
    const core::ShaderVariant& getShaderVariant() const { return m_shaderVariant; }
    
    //---------------------------------------------------------------------
    core::ShaderVariant& getMutShaderVariant() { return m_shaderVariant; }

private:
    //---------------------------------------------------------------------
    void computeShaderVariant()
    {
        m_shaderVariant.clear();

        if (m_material != nullptr) {
            for (auto& texture : m_material->m_textures) {
                std::string texName = texture.first;
                std::transform(texName.begin(), texName.end(), texName.begin(), ::toupper);
            
                m_shaderVariant.addDefine("HAS_" + texName);
            }
        }

        for (auto& attribute : m_vertexAttributes) {
            std::string attName = attribute.first;
            std::transform(attName.begin(), attName.end(), attName.begin(), ::toupper);

            m_shaderVariant.addDefine("HAS_" + attName);
        }
    }

public:
    uint32_t m_indexOffset    = 0;
    uint32_t m_verticesOffest = 0;
    uint32_t m_vertexIndices  = 0;

    std::unordered_map<std::string, core::Buffer> m_vertexBuffer;

    std::unique_ptr<core::Buffer> m_indexBuffer;

private:
    std::unordered_map<std::string, VertexAttribute> m_vertexAttributes;

    const Material* m_material{ nullptr };

    core::ShaderVariant m_shaderVariant;
};

} // namespace sg
} // namespace vkb