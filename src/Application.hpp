#pragma once

// we are adding all the includes to make sure all of them are correctly set up
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <imgui.h>

#include <vector>



class Application
{
public:
    void Run()
    {
        InitWindow(); InitVulkan();
        Gameloop();
        CleanUp(); // checks for user quit function from glfw
    }

private:
    void InitVulkan()
    {
        CreateVulkanInstance();
        SetupDebugger();
    }
    void InitWindow();
    void Gameloop();
    void CleanUp();


    void CreateVulkanInstance();
    void SetupDebugger();

    bool checkValidationLayerSupport();
    bool checkExtensionsSupport(const char* glfwExtensionNames);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);
    std::vector<const char*> getRequiredExtensions();


private:
    const uint32_t WindowWidth = 1280;
    const uint32_t WindowHeight = 720;
    GLFWwindow* m_Window;
    
    VkInstance m_VkInstance;
    const std::vector<const char*> vulkanValidationLayers {
        "VK_LAYER_KHRONOS_validation"
    };
    #ifdef VF_DEBUG
    const bool enableValidationLayers = true;
    #elif VF_RELEASE
    const bool enableValidationLayers = false;
    #endif

    VkDebugUtilsMessengerEXT m_DebugMessenger;
};
