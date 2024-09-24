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
    if(m_enableValidationLayers)
        Debugger::DestroyDebugUtilMessengerEXT(m_VkInstance, nullptr, m_DebugMessenger);

    vkDestroyDevice(m_VkDevice, nullptr);

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
    if(m_enableValidationLayers)
    {
        vkInstanceInfo.enabledLayerCount = static_cast<uint32_t>(m_VulkanValidationLayers.size());
        vkInstanceInfo.ppEnabledLayerNames = m_VulkanValidationLayers.data();

        populateDebugMessengerCreateInfo(vkDebugMessengerCreateInfo);
        vkInstanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &vkDebugMessengerCreateInfo;
    }
    vkInstanceInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanExtensions.size());
    vkInstanceInfo.ppEnabledExtensionNames = VulkanExtensions.data();

    if(vkCreateInstance(&vkInstanceInfo, nullptr, &m_VkInstance) != VK_SUCCESS)
        throw std::runtime_error("Failed To Initialize Vulkan Instance");

    // validate the intance by checking if the needed extensions and layers are 
    if(!checkExtensionsSupport(*VulkanExtensions.data()))
        throw std::runtime_error("The Needed Extensions Failed To Initialize");
    if(m_enableValidationLayers && !checkValidationLayerSupport())
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
std::vector<const char*> Application::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensionNames;

    glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensionNames, glfwExtensionNames + glfwExtensionCount);

    if(m_enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}


bool Application::checkValidationLayerSupport()
{
    uint32_t ValidationLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&ValidationLayerCount, nullptr);

    std::vector<VkLayerProperties> ValidationLayerProperties(ValidationLayerCount);
    vkEnumerateInstanceLayerProperties(&ValidationLayerCount, ValidationLayerProperties.data());

    // check for the layers we requested
    for(const char* layerName : m_VulkanValidationLayers)
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


void Application::SetupDebugger()
{
    if(!m_enableValidationLayers) return;

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


void Application::SetPhysicalDevice()
{
    // first we check how many phy devices we have
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_VkInstance, &physicalDeviceCount, nullptr);

    if(physicalDeviceCount == 0)
        throw std::runtime_error("Couldn't Find A Single Physical Device");

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(m_VkInstance, &physicalDeviceCount, physicalDevices.data());

    // checks all the devices and gets a suitable phy device
    for(const VkPhysicalDevice& vkpd : physicalDevices)
    {
        if(isSuitablePhyDevice(vkpd))
        {
            m_VkPhysicalDevice = vkpd;
            break;
        }
    }
    if (m_VkPhysicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("Couldn't Find A GPU Device That Satisfies My Needs");
    else
        std::cout << "Using " << m_PhysicalDeviceProperties.deviceName << " As The Physical Device" << std::endl;
}
bool Application::isSuitablePhyDevice(VkPhysicalDevice phyDevice)
{
    QueueFamilyIndices qfIndices = findQueueFamilies(phyDevice);

    vkGetPhysicalDeviceProperties(phyDevice, &m_PhysicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(phyDevice, &m_PhysicalDeviceFeatures);

    return
        m_PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        m_PhysicalDeviceFeatures.geometryShader &&
        qfIndices.isComplete()
    ;
}
Application::QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice phyDevice)
{
    QueueFamilyIndices qfIndices;

    uint32_t qfCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &qfCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(qfCount);
    vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &qfCount, queueFamilies.data());

    int QueueIndice = 0;
    for (const VkQueueFamilyProperties& qfp : queueFamilies)
    {
        if (qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            qfIndices.graphicsFamily = QueueIndice;
        if (qfIndices.isComplete())
            break;

        QueueIndice++;
    }

    return qfIndices;
}


void Application::CreateLogicalDevice()
{
    QueueFamilyIndices qfIndices = findQueueFamilies(m_VkPhysicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = qfIndices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;


    VkPhysicalDeviceFeatures phyDeviceFeatures{};


    VkDeviceCreateInfo logicalDeviceCreateInfo{};
    logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logicalDeviceCreateInfo.queueCreateInfoCount = 1;
    logicalDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    logicalDeviceCreateInfo.pEnabledFeatures = &phyDeviceFeatures;

    logicalDeviceCreateInfo.enabledExtensionCount = 0;

    // i dont know if there is still support for device level validation layers.
    if (m_enableValidationLayers)
    {
        logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_VulkanValidationLayers.size());
        logicalDeviceCreateInfo.ppEnabledLayerNames = m_VulkanValidationLayers.data();
    }
    else
        logicalDeviceCreateInfo.enabledLayerCount = 0;

    
    if (vkCreateDevice(m_VkPhysicalDevice, &logicalDeviceCreateInfo, nullptr, &m_VkDevice) != VK_SUCCESS)
        throw std::runtime_error("Failed To Create A Logical Device");
    else
        vkGetDeviceQueue(m_VkDevice, qfIndices.graphicsFamily.value(), 0, &m_GraphicsFamilyQueue);
}