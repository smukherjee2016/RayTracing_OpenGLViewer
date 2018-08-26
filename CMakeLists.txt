cmake_minimum_required(VERSION 3.8)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


project(RayTracing_OpenGLViewer VERSION 0.1.0 LANGUAGES CXX)



find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/glew/build/cmake/CMakeLists.txt"
        OR NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/glfw/CMakeLists.txt"
            OR NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/glm/CMakeLists.txt")
        message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

#GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

#GLEW
set(BUILD_UTILS OFF CACHE BOOL "")

#GLM
set(GLM_TEST_ENABLE OFF CACHE BOOL "")


add_subdirectory("extern/glfw")
add_subdirectory("extern/glew")
add_subdirectory("extern/glm")

include_directories("extern/glfw/include/")
include_directories("extern/glew/include/")
include_directories("extern/glm/")



add_library(RayTracing_OpenGLViewer_lib include/RayTracing_OpenGLViewer.hpp)
target_include_directories(RayTracing_OpenGLViewer_lib PUBLIC include)
target_compile_features(RayTracing_OpenGLViewer_lib PUBLIC cxx_std_14)
set_target_properties(RayTracing_OpenGLViewer_lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(RayTracing_OpenGLViewer_lib glfw ${GLFW_LIBRARIES})

add_executable(RayTracing_OpenGLViewer_exe src/RayTracing_OpenGLViewer.cpp)
target_link_libraries(RayTracing_OpenGLViewer_exe PUBLIC RayTracing_OpenGLViewer_lib)