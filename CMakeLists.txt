cmake_minimum_required(VERSION 3.10)


# variables
set(ProjectName Vulfus)


# creating the project
project(${ProjectName})

# setting the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "./bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "./bin/objects/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")

# now we set the c++ standards
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

# this setting exports json file that helps the code editor's intellisence
# basicly it will give the intellisence the sence to work corrently
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Finding vulkan
find_package(Vulkan REQUIRED)


# some crossplatform stuff to make decisions based on what os we are using
# these cmds tell the compiler what platform it is
if(WIN32)
    add_compile_definitions(VF_WINDOWS)
elseif(UNIX)
    add_compile_definitions(VF_UNIX)
endif()


# now we add some build types
# we set the build type to debug by def, but we can choose if we want the build type to be Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Choose a build type" FORCE)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose a build type (Debug | Release)" FORCE)
endif()



# time to add the submodules
# GLFW
add_subdirectory(./submodules/glfw)

# adding the sources for our executable
file(GLOB_RECURSE MAIN_SOURCES "./src/*.cpp" "./src/*.hpp")
file(GLOB_RECURSE GLM_SOURCES "./submodules/glm/glm/*.hpp")


# adding the include directories
include_directories(
    src
    submodules/glfw/include
    submodules/glm/glm
    submodules/imgui
)


# CREATING APPLICATION #
add_executable(${ProjectName} ${MAIN_SOURCES} ${GLM_SOURCES})


# adding libraries
# linking the vulkan library
target_include_directories(${ProjectName} PRIVATE ${Vulkan_INCLUDE_DIR})
target_link_libraries(${ProjectName} PRIVATE ${Vulkan_LIBRARIES} glfw)


# adding compile definitions
target_compile_definitions(${ProjectName} PUBLIC GLFW_INCLUDE_VULKAN)