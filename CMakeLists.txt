cmake_minimum_required(VERSION 3.6)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()

project(RayTracing_OpenGLViewer VERSION 0.1.0 LANGUAGES CXX)

add_library(RayTracing_OpenGLViewer_lib include/RayTracing_OpenGLViewer.hpp)
target_include_directories(RayTracing_OpenGLViewer_lib PUBLIC include)
target_compile_features(RayTracing_OpenGLViewer_lib PUBLIC cxx_std_14)
set_target_properties(RayTracing_OpenGLViewer_lib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(RayTracing_OpenGLViewer_exe src/RayTracing_OpenGLViewer.cpp)
target_link_libraries(RayTracing_OpenGLViewer_exe PUBLIC RayTracing_OpenGLViewer_lib)