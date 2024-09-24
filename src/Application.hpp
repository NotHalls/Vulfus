#pragma once

// we are adding all the includes to make sure all of them are correctly set up
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <imgui.h>

#include <vector>
#include <optional>



class Application
{
private:
    // Window Stuff //
    const uint32_t WindowWidth = 1280;
    const uint32_t WindowHeight = 720;
    GLFWwindow* m_Window;



    // Vulkan Main Properties //
    VkInstance m_VkInstance = VK_NULL_HANDLE;
    VkPhysicalDevice m_VkPhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_VkDevice = VK_NULL_HANDLE;

    // Vulkan Queues
    VkQueue m_GraphicsFamilyQueue;



    // Vulkan Layers
    const std::vector<const char*> m_VulkanValidationLayers {
        "VK_LAYER_KHRONOS_validation"
    };



    // Vulkan Debugging Stuff //
#ifdef VF_DEBUG
    const bool m_enableValidationLayers = true;
#elif VF_RELEASE
    const bool m_enableValidationLayers = false;
#endif
    VkDebugUtilsMessengerEXT m_DebugMessenger;



    // Vulkan Device Stuff //
    VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
    VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures;
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value();
        }
    };





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
        SetPhysicalDevice();
        CreateLogicalDevice();
    }
    void InitWindow();
    void Gameloop();
    void CleanUp();



    void CreateVulkanInstance();
    void SetupDebugger();
    void SetPhysicalDevice();
    void CreateLogicalDevice();

    
    bool checkValidationLayerSupport();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);
    
    bool checkExtensionsSupport(const char* glfwExtensionNames);
    std::vector<const char*> getRequiredExtensions();
    
    
    bool isSuitablePhyDevice(VkPhysicalDevice phyDevice);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice phyDevice);
};