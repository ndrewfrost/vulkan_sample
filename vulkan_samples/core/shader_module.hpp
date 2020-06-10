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

//-------------------------------------------------------------------------
// Types of shader resources
//
enum class ShaderResourceType
{
    Input, InputAttachment, Output, Image, ImageSampler, ImageStorage, Sampler, 
    BufferUniform, BufferStorage, PushConstant, SpecializationConstant, All
};

//-------------------------------------------------------------------------
// Determines the type and method of how descriptor set
// should be created and bound
//
enum class ShaderResourceMode
{
    Static, Dynamic, UpdateAfterBind
};

//-------------------------------------------------------------------------
// Store shader resource data
// used by shader module
//
struct ShaderResource
{
    vk::ShaderStageFlagBits stages;
    ShaderResourceType      type;
    ShaderResourceMode      mode;
    uint32_t                set;
    uint32_t                binding;
    uint32_t                location;
    uint32_t                inputAttachmentIndex;
    uint32_t                vecSize;
    uint32_t                columns;
    uint32_t                arraySize;
    uint32_t                offset;
    uint32_t                size;
    uint32_t                constantId;
    std::string             name;

};

///////////////////////////////////////////////////////////////////////////
// ShaderVariant                                                         //
///////////////////////////////////////////////////////////////////////////
// Adds support for c style preprocessor macros to glsl shaders          //
// enabling you to define or undefine certain symbols                    //
///////////////////////////////////////////////////////////////////////////

class ShaderVariant
{
public:
    //---------------------------------------------------------------------
    ShaderVariant() = default;
    
    //---------------------------------------------------------------------
    ShaderVariant(std::string&& preamble, std::vector<std::string>&& processes);

    //---------------------------------------------------------------------
    size_t getId() const;

    //---------------------------------------------------------------------
    // Add definitions to shader variant
    //
    void addDefinitions(const std::vector<std::string>& definitions);


    //---------------------------------------------------------------------
    // addsa define macro to the shader
    //
    void addDefine(const std::string& def);

    //---------------------------------------------------------------------
    // adds an undef macro to the shader
    //
    void addUndefine(const std::string& undef);

    //---------------------------------------------------------------------
    // Specifies the size of a named runtime array for automatic reflection
    // if already specified, overrides the size
    //
    void addRuntimeArraySize(const std::string& runtimeArraySizeName, size_t size);

    void addRuntimeArraySizes(const std::unordered_map<std::string, size_t>& sizes);

    //---------------------------------------------------------------------
    const std::string& getPreamble() const;

    //---------------------------------------------------------------------
    const std::vector<std::string>& getProcesses() const;

    //---------------------------------------------------------------------
    const std::unordered_map<std::string, size_t>& getRuntimeArraySizes() const;

    //---------------------------------------------------------------------
    void clear();

private:
    //---------------------------------------------------------------------
    void updateId();

private:
    size_t m_id;    
    std::string m_preamble;
    std::vector<std::string> m_processes;
    std::unordered_map<std::string, size_t> m_runtimeArraySizes;
};

///////////////////////////////////////////////////////////////////////////
// ShaderSource                                                          //
///////////////////////////////////////////////////////////////////////////

class ShaderSource
{
public:
    //---------------------------------------------------------------------
    ShaderSource() = default;

    //---------------------------------------------------------------------
    ShaderSource(const std::string& filename);

    //---------------------------------------------------------------------
    ShaderSource(std::vector<uint8_t>&& data);

    //---------------------------------------------------------------------
    size_t getId() const;

    //---------------------------------------------------------------------
    const std::string& getFilename() const;

    //---------------------------------------------------------------------
    const std::vector<uint8_t>& getData() const;

private:
    size_t               m_id;
    std::string          m_filename;
    std::vector<uint8_t> m_data;
};

///////////////////////////////////////////////////////////////////////////
// ShaderModule                                                          //
///////////////////////////////////////////////////////////////////////////
// Contains shader code, with an entry point for a specific shader stage //
// nedded by a pipelineLayout to create a pipeline                       //
// ShaderModule can do auto-pairing between shader code and textures.    //
// The low level code can change bindings, just keeping the name of the  //
// texture. Variants for each texture are also generated,                //
// such as HAS_BASE_COLOR_TEX. It works similarly for attribute          //
// locations.A current limitation is that only set 0 is considered.      //
// Uniform buffers are currently hardcoded as well.                      //
///////////////////////////////////////////////////////////////////////////

class ShaderModule
{
public:
    //---------------------------------------------------------------------
    ShaderModule(vk::Device& device,
                 vk::ShaderStageFlagBits stage,
                 const ShaderSource& glsl_source,
                 const std::string& entry_point,
                 const ShaderVariant& shader_variant);

    //---------------------------------------------------------------------
    ShaderModule(const ShaderModule&) = delete;

    //---------------------------------------------------------------------
    ShaderModule(ShaderModule&& other);

    //---------------------------------------------------------------------
    ShaderModule& operator=(const ShaderModule&) = delete;

    //---------------------------------------------------------------------
    ShaderModule& operator=(ShaderModule&&) = delete;

    //---------------------------------------------------------------------
    size_t getId() const;

    //---------------------------------------------------------------------
    vk::ShaderStageFlagBits getStage() const;

    //---------------------------------------------------------------------
    const std::string& getEntryPoint() const;

    //---------------------------------------------------------------------
    const std::vector<ShaderResource>& getResources() const;

    //---------------------------------------------------------------------
    const std::string& getInfoLog() const;

    //---------------------------------------------------------------------
    const std::vector<uint32_t>& getBinary() const;

    //---------------------------------------------------------------------
    void setResourceMode(const ShaderResourceMode& mode, const std::string& resource_name);

private:
    vk::Device&                 m_device;    
    size_t                      m_id;         // Shader id    
    VkShaderStageFlagBits       m_stage{};    // Stage of shader (vertex, fragment, etc)    
    std::string                 m_entryPoint; // Name of the main func    
    std::vector<uint32_t>       m_spirv;
    std::vector<ShaderResource> m_resources;
    std::string                 m_infoLog;
};

} // namespace core 
} // namespace vkb 