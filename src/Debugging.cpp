#include "Debugging.hpp"
#include "vulkan/vulkan_core.h"

#include <iostream>



VKAPI_ATTR VkBool32 VKAPI_CALL Debugger::DebugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void *userData
){
    std::cerr << "Debugger: " << callbackData->pMessage << std::endl;

    return VK_FALSE;
}


VkResult Debugger::CreateDebugUtilMessageEXT (
    VkInstance vkInstance,
    const VkDebugUtilsMessengerCreateInfoEXT* debugMsgCreationInfo,
    const VkAllocationCallbacks* vkAllocationCallbacks,
    VkDebugUtilsMessengerEXT* debugMessenger
){
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr)
        return func(vkInstance, debugMsgCreationInfo, vkAllocationCallbacks, debugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Debugger::DestroyDebugUtilMessengerEXT(VkInstance vkInstance, VkAllocationCallbacks* vkAllocationCallbacks, VkDebugUtilsMessengerEXT debugMessenger)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr)
        func(vkInstance, debugMessenger, vkAllocationCallbacks);
}