cmake_minimum_required(VERSION 3.17)
project(Vulkan)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH $ENV{CMAKE_PREFIX_PATH})

include(CTest)
#find_package(PkgConfig REQUIRED)
enable_testing()

#pkg_search_module(GLFW REQURED glfw3)
find_library(GLFW glfw3 HINTS "${CMAKE_PREFIX_PATH}/GLFW/lib")
find_package(Vulkan REQUIRED FATAL_ERROR)
find_package(OpenGL REQUIRED)

add_subdirectory("${CMAKE_PREFIX_PATH}/Boxer" "${CMAKE_PREFIX_PATH}/Boxer/build")
add_subdirectory("${CMAKE_PREFIX_PATH}/yaml" "${CMAKE_PREFIX_PATH}/yaml/build")

include_directories("src")
include_directories(${CMAKE_PREFIX_PATH}/GLFW/include)
include_directories("${CMAKE_PREFIX_PATH}/EnTT/src/")
include_directories("${CMAKE_PREFIX_PATH}/glm")
include_directories("${CMAKE_PREFIX_PATH}/Boxer/include")
include_directories("${CMAKE_PREFIX_PATH}/Boxer/src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "../bin")

file(GLOB_RECURSE Vulkan CONFIGURE_DEPENDS "src/*.cpp" "src/*.h" )
add_executable(Vulkan WIN32 ${Vulkan})
target_link_libraries(Vulkan ${GLFW} Vulkan::Vulkan ${OpenGL} bgfx Boxer meshoptimizer yaml-cpp)