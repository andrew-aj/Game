#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

//#define GLFW_INCLUDE_VULKAN

#include "ModelLoader.h"
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

#include "Common/interface/RefCntAutoPtr.hpp"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "boxer/boxer.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"


#include "Input.h"
#include "Components.h"
#include "Scene.h"
#include "SystemManager.h"
#include "Entity.h"

#include <string>
#include <cstdint>
#include <iostream>
#include <any>

namespace SGE {

    class Engine {
    public:
        Engine();

        Engine(const std::string &name, uint32_t width, uint32_t height);

        ~Engine();

        static void resizeCallback(GLFWwindow *window, int width, int height);

        void setupEntities(entt::registry &registry, YAML::Node &node);

        bool createWindow(int glfwAPIHint);

        bool initEngine(Diligent::RENDER_DEVICE_TYPE deviceType);

        void update();

        void addSystems();

    private:
        std::string name;
        uint32_t WIDTH, HEIGHT;

        GLFWwindow *window = nullptr;
        std::shared_ptr<Entity> windowEnt;

        Scene m_scene;

        SystemManager manager = SystemManager(m_scene);
    };

    Engine::Engine() : name("Vulkan"), WIDTH(1280), HEIGHT(720) {

    }

    Engine::Engine(const std::string &name, uint32_t width, uint32_t height) : name(name), WIDTH(width),
                                                                               HEIGHT(height) {

    }

    Engine::~Engine() {
        if (DeviceClass::getContext())
            DeviceClass::getContext()->Flush();
        DeviceClass::m_pSwapChain = nullptr;
        DeviceClass::m_pImmediateContext = nullptr;
        DeviceClass::m_pDevice = nullptr;
        manager.runShutDown();
        if (window)
            glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Engine::resizeCallback(GLFWwindow *window, int width, int height) {
        Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
        windowSizeUpdate(engine->m_scene.m_world, width, height);
        if (DeviceClass::m_pSwapChain != nullptr)
            DeviceClass::m_pSwapChain->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    bool Engine::createWindow(int glfwAPIHint) {
        if (glfwInit() != GLFW_TRUE) {
            boxer::show("GLFW failed to initalize.", "Error");
            return false;
        }
#if PLATFORM_WIN32
        glfwAPIHint = GLFW_NO_API;
#elif
        if(glfwAPIHint != GLFW_OPENGL_API)
            glfwAPIHint = GLFW_NO_API
#endif
        glfwWindowHint(GLFW_CLIENT_API, glfwAPIHint);
        if (glfwAPIHint == GLFW_OPENGL_API) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        }

        window = glfwCreateWindow(static_cast<int>(WIDTH), static_cast<int>(HEIGHT), name.c_str(), nullptr, nullptr);
        if (window == nullptr) {
            boxer::show("Failed to create GLFW window.", "Error");
            return false;
        }

        Input::setUpInputs(window);
        Input::setUpMouseInputs(window);
        Input::setUpMouseButton(window);
        Input::setUpScroll(window);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, resizeCallback);
//        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        return true;
    }

    bool Engine::initEngine(Diligent::RENDER_DEVICE_TYPE deviceType) {
#if PLATFORM_WIN32
        Diligent::Win32NativeWindow Window{glfwGetWin32Window(window)};
#endif
#if PLATFORM_LINUX
        Diligent::LinuxNativeWindow Window;
        Window.WindowId = glfwGetX11Window(window);
        Window.pDisplay = glfwGetX11Display();
        if (DevType == Diligent::RENDER_DEVICE_TYPE_GL)
            glfwMakeContextCurrent(window);
#endif
#if PLATFORM_MACOS
        Diligent::MacOSNativeWindow Window;
        if (DevType == Diligent::RENDER_DEVICE_TYPE_GL)
            glfwMakeContextCurrent(window);
        else
            Window.pNSView = GetNSWindowView(window);
#endif
        Diligent::SwapChainDesc desc;
        switch (deviceType) {
#if D3D11_SUPPORTED
            case Diligent::RENDER_DEVICE_TYPE_D3D11: {
#if ENGINE_DLL
                auto* GetEngineFactoryD3D11 = Diligent::LoadGraphicsEngineD3D11();
#endif
                auto* pFactoryD3D11 = GetEngineFactoryD3D11();

                Diligent::EngineD3D11CreateInfo EngineCI;
                pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &m_pDevice, &m_pImmediateContext);
                pFactoryD3D11->CreateSwapChainD3D11(m_pDevice, m_pImmediateContext, desc, Diligent::FullScreenModeDesc{}, &m_pSwapChain);

            }
            break;
#endif

#if D3D12_SUPPORTED
            case Diligent::RENDER_DEVICE_TYPE_D3D12: {
#if ENGINE_DLL
                auto *GetEngineFactoryD3D12 = Diligent::LoadGraphicsEngineD3D12();
#endif
                auto* pFactoryD3D12 = GetEngineFactoryD3D12();

                Diligent::EngineD3D12CreateInfo EngineCI;
                pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_pDevice, &m_pImmediateContext);
                pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, desc, Diligent::FullScreenModeDesc{}, Window, &m_pSwapChain);
            }
            break;
#endif

#if GL_SUPPORTED
            case Diligent::RENDER_DEVICE_TYPE_GL: {
#if EXPLICITLY_LOAD_ENGINE_GL_DLL
                auto GetEngineFactoryOpenGL = Diligent::LoadGraphicsEngineOpengl();
#endif
                auto *pFactoryOpenGL = Diligent::GetEngineFactoryOpenGL();

                Diligent::EngineGLCreateInfo EngineCI;
                EngineCI.Window = Window;
                pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &DeviceClass::m_pDevice, &DeviceClass::m_pImmediateContext, desc,
                                                           &DeviceClass::m_pSwapChain);
            }
                break;
#endif

#if VULKAN_SUPPORTED
            case Diligent::RENDER_DEVICE_TYPE_VULKAN: {
#if EXPLICITLY_LOAD_ENGINE_VK_DLL
                auto* GetEngineFactoryVk = Diligent::LoadGraphicsEngineVk();
#endif
                auto *pFactoryVk = Diligent::GetEngineFactoryVk();

                Diligent::EngineVkCreateInfo EngineCI;
                pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &DeviceClass::m_pDevice, &DeviceClass::m_pImmediateContext);
                pFactoryVk->CreateSwapChainVk(DeviceClass::m_pDevice, DeviceClass::m_pImmediateContext, desc, Window, &DeviceClass::m_pSwapChain);
            }
                break;
#endif

#if METAL_SUPPORTED
                case Diligent::RENDER_DEVICE_TYPE_METAL:
            {
                auto* pFactoryMtl = Diligent::GetEngineFactoryMtl();

                Diligent::EngineMtlCreateInfo EngineCI;
                pFactoryMtl->CreateDeviceAndContextsMtl(EngineCI, &m_pDevice, &m_pImmediateContext);
                pFactoryMtl->CreateSwapChainMtl(m_pDevice, m_pImmediateContext, desc, Window, &m_pSwapChain);
            }
            break;
#endif

            default:
                std::cerr << "Unknown/unsupported device type";
                return false;
                break;
        }

        if (DeviceClass::m_pDevice == nullptr || DeviceClass::m_pImmediateContext == nullptr || DeviceClass::m_pSwapChain == nullptr)
            return false;

        YAML::Node node = YAML::LoadFile("data/systems/entities.yml");
        node = node["Entities"];
        setupEntities(m_scene.m_world, node);

        entt::entity tempEntt = node["Window"]["PregenID"].as<entt::entity>();
        auto &windPtr = m_scene.m_world.get<WindowPtr>(tempEntt);
        windPtr.window = window;
        windPtr.m_Device = DeviceClass::m_pDevice;
        windPtr.m_ImmediateContext = DeviceClass::m_pImmediateContext;
        windPtr.m_SwapChain = DeviceClass::m_pSwapChain;

        windowEnt = std::make_shared<Entity>(tempEntt, &m_scene.m_world);
        addSystems();
        manager.runStartUp();

        return true;
    }

    void Engine::update() {
        auto &comp = m_scene.m_world.get<WindowPtr>(windowEnt->operator entt::entity());
        while (!glfwWindowShouldClose(window) && comp.running) {
            glfwPollEvents();

            Diligent::ITextureView *pRTV = DeviceClass::m_pSwapChain->GetCurrentBackBufferRTV();
            DeviceClass::m_pImmediateContext->SetRenderTargets(1, &pRTV, nullptr,
                                                  Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

            const float clearColor[4] = {};
            DeviceClass::m_pImmediateContext->ClearRenderTarget(pRTV, clearColor, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

            manager.runSystems();

            DeviceClass::m_pImmediateContext->Flush();
            DeviceClass::m_pSwapChain->Present();
        }
    }

    void Engine::addSystems() {
//        manager.registerSystem(new MeshModelLoader(), 0);
//        manager.registerSystem(new GraphicsUnloader(), 0);
//        manager.registerSystem(new GameTime(), 0);
//        manager.registerSystem(new Camera(), 10);
//        manager.registerSystem(new UpdateMovement(), 5);
//        manager.registerSystem(new PrimaryMovement(), 4);
//        manager.registerSystem(new Renderer(), manager.min_priority);
//        manager.registerSystem(new CloseEngine(), 0);
    }

    void Engine::setupEntities(entt::registry &registry, YAML::Node &node) {
        std::unordered_map<std::string, std::any> stringToComp;
        stringToComp["ModelComponent"] = ModelComponent();
        stringToComp["Vertex"] = Vertex();
        stringToComp["MeshComponent"] = MeshComponent();
        stringToComp["Program"] = Program();
        stringToComp["VertexBuffer"] = VertexBuffer();
        stringToComp["IndexBuffer"] = IndexBuffer();
        stringToComp["MovementDisabled"] = MovementDisabled();

        for (auto it = node.begin(); it != node.end(); it++) {
            auto &sub = it->second;
            entt::entity entity = registry.create((entt::entity) sub["PregenID"].as<uint32_t>());
            if (sub["CameraComponent"])
                registry.emplace<CameraComponent>(entity) = sub["CameraComponent"].as<CameraComponent>();
            if (sub["Transform"])
                registry.emplace<Transform>(entity) = sub["Transform"].as<Transform>();
            if (sub["Physics"])
                registry.emplace<Physics>(entity) = sub["Physics"].as<Physics>();
            if (sub["AttachedTo"])
                registry.emplace<AttachedTo>(entity) = sub["AttachedTo"].as<AttachedTo>();
            if (sub["PrimaryController"])
                registry.emplace<PrimaryController>(entity) = sub["PrimaryController"].as<PrimaryController>();
            if (sub["Tag"])
                registry.emplace<Tag>(entity) = sub["Tag"].as<Tag>();
            if (sub["Time"])
                registry.emplace<Time>(entity) = sub["Time"].as<Time>();
            if (sub["WindowPtr"])
                registry.emplace<WindowPtr>(entity) = sub["WindowPtr"].as<WindowPtr>();
            if (sub["UIComponent"])
                registry.emplace<UIComponent>(entity) = sub["UIComponent"].as<UIComponent>();
        }
    }


}


#endif //VULKAN_ENGINE_H
