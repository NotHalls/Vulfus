#pragma once

#include "vulkan/vk_platform.h"
#include "vulkan/vulkan_core.h"



class Debugger
{
public:
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback (
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData // optional
    );


    static VkResult CreateDebugUtilMessageEXT (
        VkInstance vkInstance,
        const VkDebugUtilsMessengerCreateInfoEXT* debugMsgCreationInfo,
        const VkAllocationCallbacks* vkAllocationCallbacks,
        VkDebugUtilsMessengerEXT* debugMessenger
    );
    static void DestroyDebugUtilMessengerEXT (VkInstance vkInstance, VkAllocationCallbacks* vkAllocationCallbacks, VkDebugUtilsMessengerEXT debugMessenger);
};