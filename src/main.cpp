// we are adding all the includes to make sure all of them are correctly set up
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <imgui.h>

// we need these for the base c++ functions in the code
// well you know that already.
#include <iostream>
#include <cstdlib>
#include <exception>
#include <cstdint>
#include <stdexcept>
#include <vector>



class MainWindow
{
public:
    void run()
    {
        InitWindow(); InitVulkan();
        Gameloop();
        CleanUp(); // checks for user quit function from glfw
    }


private:
// we have three functions to init, run the loop, and quit/cleanup the mess
// you can tell what they do by just looking at them
    void InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // this tells glfw not to create an openGL context as we are using vulkan and not openGL
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // resizing will be suffered with later

        window = glfwCreateWindow(WindowWidth, WindowHeight, "Vulfus", nullptr, nullptr);
    }

    void InitVulkan()
    {
        CreateVkInstance();
    }

    void Gameloop()
    {
        while(!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
    }

    void CleanUp()
    {
        vkDestroyInstance(vkInstance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void CreateVkInstance()
    {
        // creating the application info
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
        uint32_t glfwExtensionCount;
        const char** glfwExtensionNames;

        glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        
        vkInstanceInfo.enabledExtensionCount = glfwExtensionCount;
        vkInstanceInfo.ppEnabledExtensionNames = glfwExtensionNames;

        if(vkCreateInstance(&vkInstanceInfo, nullptr, &vkInstance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create Vulkan Instance");
        }

        
        // shows the extensions needed for vulkan
        uint32_t vkExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);

        std::vector<VkExtensionProperties> vkExtensions(vkExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtensions.data());

        std::cout << "Extensions needed:\n";
        for(const VkExtensionProperties& vkx : vkExtensions)
        {
            std::cout << "\t" << vkx.extensionName << "\n";
        }
        

        if(CheckAllExtensions(glfwExtensionCount, vkExtensionCount) == false)
        {
            std::cout << "WARN: fuck, some or all extensions needed by vulkan were not added by glfw" << std::endl;
        }
    }

    bool CheckAllExtensions(const uint32_t glfwExt, const uint32_t vkExt)
    { return glfwExt == vkExt ? true : false; }
    
private:
    GLFWwindow* window;
    VkInstance vkInstance;

    const uint32_t WindowWidth = 1280;
    const uint32_t WindowHeight = 720;
};

int main()
{
    int a = 5;

    MainWindow APP;

    // basicly we run the application process and check for errors if there are any with exception handelling
    try
    {
        APP.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // i think EXIT_ looks cooler than just plain 0 or 1
    return EXIT_SUCCESS;
}