cmake_minimum_required(VERSION 3.8)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()

project(RayTracing_OpenGLViewer VERSION 0.2.0)



set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    endif()
endif()


SET(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

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

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/glfw/CMakeLists.txt"
            OR NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/glm/CMakeLists.txt")
        message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

#GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")


#GLM
set(GLM_TEST_ENABLE OFF CACHE BOOL "")



add_subdirectory("extern/glfw")
add_subdirectory("extern/glm")



set(GLAD "${CMAKE_CURRENT_LIST_DIR}/extern/glad/include/glad/glad.h"
		 "${CMAKE_CURRENT_LIST_DIR}/extern/glad/include/KHR/khrplatform.h"
         "${CMAKE_CURRENT_LIST_DIR}/extern/glad/src/glad.c"
        )

add_library(RayTracing_OpenGLViewer_lib INTERFACE)
target_sources( RayTracing_OpenGLViewer_lib
        INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/include/RayTracing_OpenGLViewer.hpp"
        ${GLAD}
)
target_include_directories(RayTracing_OpenGLViewer_lib INTERFACE "${CMAKE_CURRENT_LIST_DIR}/extern/glfw/include/")
target_include_directories(RayTracing_OpenGLViewer_lib INTERFACE "${CMAKE_CURRENT_LIST_DIR}/extern/glad/include/")
target_include_directories(RayTracing_OpenGLViewer_lib INTERFACE "${CMAKE_CURRENT_LIST_DIR}/extern/glm/")
#set_target_properties(RayTracing_OpenGLViewer_lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(RayTracing_OpenGLViewer_lib INTERFACE include/)
target_link_libraries(RayTracing_OpenGLViewer_lib INTERFACE glfw ${GLFW_LIBRARIES})


add_executable(RayTracing_OpenGLViewer_exe src/RayTracing_OpenGLViewer.cpp)
target_link_libraries(RayTracing_OpenGLViewer_exe PUBLIC RayTracing_OpenGLViewer_lib)