#include "Engine.h"

namespace SGE{
    DeviceClass* Engine::m_deviceClass;

    Engine::Engine() : m_name("Vulkan"), WIDTH(1280), HEIGHT(720) {

    }

    Engine::Engine(const std::string &name, uint32_t width, uint32_t height) : m_name(name), WIDTH(width),
                                                                               HEIGHT(height) {

    }

    Engine::~Engine() {
        if (m_deviceClass->m_pImmediateContext)
            m_deviceClass->m_pImmediateContext->Flush();
        m_deviceClass->m_pSwapChain = nullptr;
        m_deviceClass->m_pImmediateContext = nullptr;
        m_deviceClass->m_pDevice = nullptr;
        m_manager.runShutDown();
        if (window)
            glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Engine::resizeCallback(GLFWwindow *window, int width, int height) {
        Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
        if (m_deviceClass->m_pSwapChain != nullptr)
            m_deviceClass->m_pSwapChain->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    bool Engine::createWindow(int glfwAPIHint) {
        m_deviceClass = DeviceClass::getInstance();

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

        window = glfwCreateWindow(static_cast<int>(WIDTH), static_cast<int>(HEIGHT), m_name.c_str(), nullptr, nullptr);
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
                pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &m_deviceClass->m_pDevice, &m_deviceClass->m_pImmediateContext, desc,
                                                           &m_deviceClass->m_pSwapChain);
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
                pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_deviceClass->m_pDevice, &m_deviceClass->m_pImmediateContext);
                pFactoryVk->CreateSwapChainVk(m_deviceClass->m_pDevice, m_deviceClass->m_pImmediateContext, desc, Window, &m_deviceClass->m_pSwapChain);
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

        if (m_deviceClass->m_pDevice == nullptr |m_deviceClass->m_pImmediateContext == nullptr |m_deviceClass->m_pSwapChain == nullptr)
            return false;

        YAML::Node node = YAML::LoadFile("data/systems/entities.yml");
        node = node["Entities"];
        setupEntities(node);

        windowEnt = node["Window"]["PregenID"].as<entt::entity>();
        auto &windPtr = m_registry.get<WindowPtr>(windowEnt);
        windPtr.window = window;
        windPtr.m_Device = m_deviceClass->m_pDevice;
        windPtr.m_ImmediateContext = m_deviceClass->m_pImmediateContext;
        windPtr.m_SwapChain = m_deviceClass->m_pSwapChain;

        startSystems();
        m_manager.runStartUp();

        return true;
    }

    void Engine::update() {
        auto &comp = m_registry.get<WindowPtr>(windowEnt);
        while (!glfwWindowShouldClose(window) && comp.running) {
            glfwPollEvents();

            Diligent::ITextureView *pRTV = m_deviceClass->m_pSwapChain->GetCurrentBackBufferRTV();
            m_deviceClass->m_pImmediateContext->SetRenderTargets(1, &pRTV, nullptr,
                                                               Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

            const float clearColor[4] = {};
            m_deviceClass->m_pImmediateContext->ClearRenderTarget(pRTV, clearColor, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);

            m_manager.runSystems();

            m_deviceClass->m_pImmediateContext->Flush();
            m_deviceClass->m_pSwapChain->Present();
        }
    }

    void Engine::startSystems() {
        m_manager.startSystems();
    }

    void Engine::setupEntities(YAML::Node &node) {
        for (auto it = node.begin(); it != node.end(); it++) {
            auto &sub = it->second;
            entt::entity entity = m_registry.create((entt::entity) sub["PregenID"].as<uint32_t>());
            if (sub["CameraComponent"])
                m_registry.emplace<CameraComponent>(entity) = sub["CameraComponent"].as<CameraComponent>();
            if (sub["Transform"])
                m_registry.emplace<Transform>(entity) = sub["Transform"].as<Transform>();
            if (sub["Physics"])
                m_registry.emplace<Physics>(entity) = sub["Physics"].as<Physics>();
            if (sub["AttachedTo"])
                m_registry.emplace<AttachedTo>(entity) = sub["AttachedTo"].as<AttachedTo>();
            if (sub["PrimaryController"])
                m_registry.emplace<PrimaryController>(entity) = sub["PrimaryController"].as<PrimaryController>();
            if (sub["Tag"])
                m_registry.emplace<Tag>(entity) = sub["Tag"].as<Tag>();
            if (sub["Time"])
                m_registry.emplace<Time>(entity) = sub["Time"].as<Time>();
            if (sub["WindowPtr"])
                m_registry.emplace<WindowPtr>(entity) = sub["WindowPtr"].as<WindowPtr>();
            if (sub["UIComponent"])
                m_registry.emplace<UIComponent>(entity) = sub["UIComponent"].as<UIComponent>();
        }
    }
}