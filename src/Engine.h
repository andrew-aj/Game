#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "boxer/boxer.h"

#include "Input.h"

#include <string>
#include <cstdint>

namespace SGE {

    class Engine {
    public:
        Engine();

        Engine(const std::string &name, uint32_t width, uint32_t height);

        ~Engine();

        static void resizeCallback(GLFWwindow *window, int width, int height);

        bool createWindow();

        bool initEngine();

    private:
        std::string name;
        uint32_t WIDTH, HEIGHT;

        GLFWwindow *window = nullptr;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain> swapChain;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> renderDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> deviceContext;
    };

    Engine::Engine() : name("Vulkan"), WIDTH(1280), HEIGHT(720) {

    }

    Engine::Engine(const std::string &name, uint32_t width, uint32_t height) : name(name), WIDTH(width),
                                                                               HEIGHT(height) {

    }

    Engine::~Engine() {

    }

    void Engine::resizeCallback(GLFWwindow *window, int width, int height) {
        Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
        if (engine->swapChain != nullptr) {
            engine->swapChain->Resize((uint32_t) width, (uint32_t) height);
        }
    }

    bool Engine::createWindow() {
        if (glfwInit() != GLFW_TRUE) {
            boxer::show("GLFW failed to initalize.", "Error");
            return false;
        }

        window = glfwCreateWindow(WIDTH, HEIGHT, name.c_str(), nullptr, nullptr);
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

        return true;
    }

    bool Engine::initEngine() {
#if PLATFORM_WIN32
        Diligent::Win32NativeWindow Window{glfwGetWin32Window(window)};
#endif
#if PLATFORM_LINUX
        Diligent::LinuxNativeWindow Window;
    Window.WindowId = glfwGetX11Window(window);
    Window.pDisplay = glfwGetX11Display();
    if (DevType == RENDER_DEVICE_TYPE_GL)
        glfwMakeContextCurrent(window);
#endif
#if PLATFORM_MACOS
        Diligent::MacOSNativeWindow Window;
    if (DevType == RENDER_DEVICE_TYPE_GL)
        glfwMakeContextCurrent(window);
    else
        Window.pNSView = GetNSWindowView(window);
#endif
        Diligent::SwapChainDesc SCDesc;
        auto *factoryVk = Diligent::GetEngineFactoryVk();
        Diligent::EngineVkCreateInfo EngineCI;
        factoryVk->CreateDeviceAndContextsVk(EngineCI, &renderDevice, &deviceContext);
        factoryVk->CreateSwapChainVk(renderDevice, deviceContext, SCDesc, Window, &swapChain);

        if(renderDevice == nullptr || deviceContext == nullptr || swapChain == nullptr)
            return false;

        return true;
    }

}


#endif //VULKAN_ENGINE_H
