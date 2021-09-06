#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

#if LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "boxer/boxer.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/bx.h"

#include "Input.h"
#include "Components.h"
#include "Scene.h"
#include "SystemManager.h"
#include "Entity.h"

#include <string>
#include <cstdint>
#include <iostream>

namespace SGE {

    class Engine {
    public:
        Engine();

        Engine(const std::string &name, uint32_t width, uint32_t height);

        ~Engine();

        static void resizeCallback(GLFWwindow *window, int width, int height);

        bool createWindow();

        bool initEngine();

        void update();

    private:
        std::string name;
        uint32_t WIDTH, HEIGHT;

        GLFWwindow *window = nullptr;
        std::shared_ptr<Entity> windowEnt;

        Scene m_scene;

        SystemManager manager = SystemManager(m_scene);

        static const bgfx::ViewId kClearView = 0;
    };

    Engine::Engine() : name("Vulkan"), WIDTH(1280), HEIGHT(720) {

    }

    Engine::Engine(const std::string &name, uint32_t width, uint32_t height) : name(name), WIDTH(width),
                                                                               HEIGHT(height) {

    }

    Engine::~Engine() {
        bgfx::shutdown();
        glfwTerminate();
    }

    void Engine::resizeCallback(GLFWwindow *window, int width, int height) {
        Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
        windowSizeUpdate(engine->m_scene.m_world, width, height);
        bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
        bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
    }

    bool Engine::createWindow() {
        if (glfwInit() != GLFW_TRUE) {
            boxer::show("GLFW failed to initalize.", "Error");
            return false;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

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

        windowEnt = std::make_shared<Entity>(m_scene.createEntity());
        windowEnt->addComponent<Tag, std::string>("Primary Window");
        windowEnt->addComponent<WindowPtr>(window, false);
        return true;
    }

    bool Engine::initEngine() {
        bgfx::renderFrame();
        bgfx::Init init;
#if WIN32
        init.platformData.nwh = glfwGetWin32Window(window);
#elif MACOS
        init.platformData.nwh = glfwGetCocoaWindow(window);
#elif LINUX
        init.platformData.ndt = glfwGetX11Display();
        init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#endif

        init.resolution.width = WIDTH;
        init.resolution.height = HEIGHT;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.type = bgfx::RendererType::Vulkan;
        if(!bgfx::init(init)) {
            boxer::show("Failed to initialize BGFX!", "Error!");
            return false;
        }
        bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
        bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

        return true;
    }

    void Engine::update(){
        while(!glfwWindowShouldClose(window)){
            glfwPollEvents();
            bgfx::touch(kClearView);
            while(manager.canTick()){
                manager.tickSystem();
            }
            bgfx::frame();
        }
    }


}


#endif //VULKAN_ENGINE_H
