#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

//#define GLFW_INCLUDE_VULKAN

#ifndef ENGINE_DLL
#    define ENGINE_DLL 1
#endif

#ifndef D3D11_SUPPORTED
#    define D3D11_SUPPORTED 0
#endif

#ifndef D3D12_SUPPORTED
#    define D3D12_SUPPORTED 0
#endif

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 0
#endif

#ifndef VULKAN_SUPPORTED
#    define VULKAN_SUPPORTED 0
#endif

#ifndef METAL_SUPPORTED
#    define METAL_SUPPORTED 0
#endif

#if PLATFORM_WIN32
#    define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#if PLATFORM_LINUX
#    define GLFW_EXPOSE_NATIVE_X11 1
#endif

#if PLATFORM_MACOS
#    define GLFW_EXPOSE_NATIVE_COCOA 1
#endif

#if D3D11_SUPPORTED
#    include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#endif
#if D3D12_SUPPORTED
#    include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#endif
#if GL_SUPPORTED

#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"

#endif
#if VULKAN_SUPPORTED

#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"

#endif
#if METAL_SUPPORTED
#    include "Graphics/GraphicsEngineMetal/interface/EngineFactoryMtl.h"
#endif

#if PLATFORM_WIN32
#    undef GetObject
#    undef CreateWindow
#endif

#include <string>
#include <cstdint>
#include <iostream>
#include <any>

#include "SystemManager.h"
#include "Input.h"
#include "Components.h"
#include "Includes.h"

namespace SGE {

    class Engine {
    public:
        Engine();

        Engine(const std::string &name, uint32_t width, uint32_t height);

        ~Engine();

        static void resizeCallback(GLFWwindow *window, int width, int height);

        void setupEntities(YAML::Node &node);

        bool createWindow(int glfwAPIHint);

        bool initEngine(Diligent::RENDER_DEVICE_TYPE deviceType);

        void update();

        void startSystems();

    private:
        std::string m_name;
        uint32_t WIDTH, HEIGHT;

        GLFWwindow *window = nullptr;
        entt::entity windowEnt;

        static DeviceClass* m_deviceClass;

        entt::registry m_registry;

        SystemManager m_manager = SystemManager(m_registry);
    };

}


#endif //VULKAN_ENGINE_H
