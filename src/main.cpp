#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm.hpp>
#include <imgui.h>

#include <iostream>

int main()
{
#ifdef VF_WINDOWS
std::cout << "windows" << std::endl;
#elif VF_UNIX
std::cout << "unix" << std::endl;
#endif
}