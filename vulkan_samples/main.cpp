/*
 *
 * Andrew Frost
 * main.cpp
 * 2020
 *
 */

#include "example_vulkan.hpp"
#include "example_vulkan.hpp"

///////////////////////////////////////////////////////////////////////////
// Main / Entry Point                                                    //
///////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    (void)(argc), (void)(argv);
    
    try {
        vkb::VkExample Example;
        Example.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}