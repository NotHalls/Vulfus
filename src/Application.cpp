#include "Application.hpp"
#include "Debugging.hpp"
#include "vulkan/vulkan_core.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cstdint>
#include <stdexcept>

#include "Debugging.hpp"



void Application::InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // this tells glfw not to create an openGL context as we are using vulkan and not openGL
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // resizing will be suffered with later

    m_Window = glfwCreateWindow(WindowWidth, WindowHeight, "Vulfus", nullptr, nullptr);
}

void Application::Gameloop()
{
    while(!glfwWindowShouldClose(m_Window))
    {
        glfwPollEvents();
    }
}


void Application::CleanUp()
{
    if(enableValidationLayers)
        Debugger::DestroyDebugUtilMessengerEXT(m_VkInstance, nullptr, m_DebugMessenger);

    vkDestroyInstance(m_VkInstance, nullptr);
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}




// creation functions
void Application::CreateVulkanInstance()
{
// creating the vulkan application info
    VkApplicationInfo vkAppInfo{};
    vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vkAppInfo.pApplicationName = "Vulfus";
    vkAppInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    vkAppInfo.pEngineName = nullptr;
    vkAppInfo.engineVersion = 0;
    vkAppInfo.apiVersion = VK_API_VERSION_1_3;

    // creating the instance info
    VkInstanceCreateInfo vkInstanceInfo{};
    vkInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkInstanceInfo.pApplicationInfo = &vkAppInfo;

    // adding extensions to vulkan
    // we will ask glfw what extensions we need and add those to vk
    std::vector<const char*> VulkanExtensions = getRequiredExtensions();

    // adds validation layers to vulkan if we are in debug mode
    // also adds the vulkan validation layers extension
    VkDebugUtilsMessengerCreateInfoEXT vkDebugMessengerCreateInfo{};
    if(enableValidationLayers)
    {
        vkInstanceInfo.enabledLayerCount = static_cast<uint32_t>(vulkanValidationLayers.size());
        vkInstanceInfo.ppEnabledLayerNames = vulkanValidationLayers.data();

        populateDebugMessengerCreateInfo(vkDebugMessengerCreateInfo);
        vkInstanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &vkDebugMessengerCreateInfo;
    }
    vkInstanceInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanExtensions.size());
    vkInstanceInfo.ppEnabledExtensionNames = VulkanExtensions.data();

    if(vkCreateInstance(&vkInstanceInfo, nullptr, &m_VkInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed To Initialize Vulkan Instance");
    }

    // validate the intance by checking if the needed extensions and layers are 
    if(!checkExtensionsSupport(*VulkanExtensions.data()))
        throw std::runtime_error("The Needed Extensions Failed To Initialize");
    if(enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("The Provided Layers Failed To Initialize");
}

bool Application::checkExtensionsSupport(const char* glfwExtensionNames)
{
    // shows the extensions needed for vulkan
    uint32_t vkExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);

    std::vector<VkExtensionProperties> vkExtensions(vkExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtensions.data());

    bool extensionFound = false;
    for(const VkExtensionProperties& extn : vkExtensions)
    {
        if(std::strcmp(glfwExtensionNames, extn.extensionName) == 0)
        {
            extensionFound = true;
            break;
        }
    }

    if(!extensionFound)
        return false;

    return true;
}

bool Application::checkValidationLayerSupport()
{
    uint32_t ValidationLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&ValidationLayerCount, nullptr);

    std::vector<VkLayerProperties> ValidationLayerProperties(ValidationLayerCount);
    vkEnumerateInstanceLayerProperties(&ValidationLayerCount, ValidationLayerProperties.data());

    // check for the layers we requested
    for(const char* layerName : vulkanValidationLayers)
    {
        bool layerFound = false;

        for(const VkLayerProperties& vkvl : ValidationLayerProperties)
        {
            if(std::strcmp(layerName, vkvl.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
            return false;

    }

    return true;
}

std::vector<const char*> Application::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensionNames;

    glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensionNames, glfwExtensionNames + glfwExtensionCount);

    if(enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}


void Application::SetupDebugger()
{
    if(!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT vkDebugCreateInfo{};
    populateDebugMessengerCreateInfo(vkDebugCreateInfo);

    if(Debugger::CreateDebugUtilMessageEXT(m_VkInstance, &vkDebugCreateInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
    { throw std::runtime_error("Failed To Initialize The Debug Messenger"); }
}
void Application::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo)
{
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = Debugger::DebugCallback;
    debugCreateInfo.pUserData = nullptr;
}