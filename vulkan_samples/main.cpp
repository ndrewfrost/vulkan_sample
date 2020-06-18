/*
 *
 * Andrew Frost
 * main.cpp
 * 2020
 *
 */

#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "common/glm_common.h"
#include "example_vulkan.hpp"

static int g_winWidth  = 800;
static int g_winHeight = 600;

//-------------------------------------------------------------------------
// GLFW on Error Callback
//
static void onErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

///////////////////////////////////////////////////////////////////////////
// Application                                                           //
///////////////////////////////////////////////////////////////////////////

void run()
{
    // Set up Window
    glfwSetErrorCallback(onErrorCallback);
    if (!glfwInit()) return;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(g_winWidth, g_winHeight, "Vulkan", nullptr, nullptr);

    // Set up Camera

    // Check Vulkan Support
    if (!glfwVulkanSupported())
        throw std::runtime_error("GLFW; Vulkan not supported");

    // Create Vulkan Base
    vkb::core::ContextCreateInfo contextInfo = {};
    contextInfo.addInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    contextInfo.addInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    contextInfo.addInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    contextInfo.addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    contextInfo.addDeviceExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
    contextInfo.addDeviceExtension(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
    contextInfo.addDeviceExtension(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
    contextInfo.addDeviceExtension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
    contextInfo.addDeviceExtension(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME);

    // Vulkan
    vkb::VkExample vkExample;
    vkExample.setupVulkan(contextInfo, window);

    // ImGui

    // Main Loop
    while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();

        // Start ImGui frame

        // update camera buffer

        // show UI window

        // start rendering the scene

        // submit for display
    }
    // cleanup
    vkExample.getDevice().waitIdle();
    vkExample.destroy();

    glfwDestroyWindow(window);
    glfwTerminate();
}

///////////////////////////////////////////////////////////////////////////
// Main / Entry Point                                                    //
///////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    (void)(argc), (void)(argv);
    
    try {
        run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}