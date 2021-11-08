#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

#if LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#define GLFW_INCLUDE_VULKAN



#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "boxer/boxer.h"

#include "Input.h"
#include "Components.h"
#include "Scene.h"
#include "SystemManager.h"
#include "Entity.h"
#include "ModelLoader.h"

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

        bool createWindow();

        bool initEngine();

        void update();

        void addSystems();

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
        manager.runShutDown();
        bgfx::shutdown();
        glfwTerminate();
    }

    void Engine::resizeCallback(GLFWwindow *window, int width, int height) {
        Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
        windowSizeUpdate(engine->m_scene.m_world, width, height);
        bgfx::reset((uint32_t) width, (uint32_t) height, BGFX_RESET_VSYNC);
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
//        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        return true;
    }

    bool Engine::initEngine() {
        init.resolution.width = WIDTH;
        init.resolution.height = HEIGHT;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.type = bgfx::RendererType::Vulkan;
        if (!bgfx::init(init)) {
            boxer::show("Failed to initialize BGFX!", "Error!");
            return false;
        }
        bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
        bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

        YAML::Node node = YAML::LoadFile("data/systems/entities.yml");
        node = node["Entities"];
        setupEntities(m_scene.m_world, node);

        entt::entity tempEntt = node["Window"]["PregenID"].as<entt::entity>();
        m_scene.m_world.get<WindowPtr>(tempEntt).window = window;

        windowEnt = std::make_shared<Entity>(tempEntt, &m_scene.m_world);
        addSystems();
        manager.runStartUp();

        return true;
    }

    void Engine::update() {
        auto &comp = m_scene.m_world.get<WindowPtr>(windowEnt->operator entt::entity());
        while (!glfwWindowShouldClose(window) && comp.running) {
            glfwPollEvents();
            bgfx::touch(kClearView);
            manager.runSystems();
            bgfx::frame();
        }
    }

    void Engine::addSystems() {
        manager.registerSystem(new MeshModelLoader(), 0);
        manager.registerSystem(new GraphicsUnloader(), 0);
        manager.registerSystem(new GameTime(), 0);
//        manager.registerSystem(new CreateTimer(), 0);
        manager.registerSystem(new Camera(), 10);
        manager.registerSystem(new UpdateMovement(), 5);
        manager.registerSystem(new primaryMovement(), 4);
        manager.registerSystem(new Renderer(), manager.min_priority);
        manager.registerSystem(new CloseEngine(), 0);
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
            auto& sub = it->second;
            entt::entity entity = registry.create((entt::entity)sub["PregenID"].as<uint32_t>());
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
