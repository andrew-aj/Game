#ifndef VULKAN_SYSTEM_H
#define VULKAN_SYSTEM_H

#include "ModelLoader.h"
#include <cassert>
#include <filesystem>
#include <execution>
#include <iostream>
#include <atomic>

#include "Input.h"
#include "CustomYaml.h"
#include "Includes.h"

namespace SGE {

    enum SystemFlag {
        EngineStart,
        EngineRunning,
        EngineStop
    };

    enum ThreadFlag {
        MultiThread = 0,
        SingleThread = 1
    };

    class System {
    public:
        void setUp(entt::registry *registry, YAML::Node &node);

        void setUp(entt::registry *registry);

        bool run();

        SystemFlag flag = EngineRunning;
        ThreadFlag threadFlag = MultiThread;
    };

    void windowSizeUpdate(entt::registry &m_registry, int width, int height) {
        for (auto &&[item, ui]: m_registry.view<UIComponent>().each()) {
            if (ui.scalingFunction) {
                ui.scalingFunction(ui.xTop, ui.yTop, ui.xBottom, ui.yBottom, ui.xScale, ui.yScale, width, height);
            }
        }
        for (auto &&[item, window] : m_registry.view<WindowPtr>().each()) {
            window.sizeChange = true;
        }
    }

    class MeshModelLoader : public System {
    public:
        MeshModelLoader() {
            flag = EngineStart;
        }

        void setUp(entt::registry *registry, YAML::Node &node) {
            node = node["MeshModelLoader"];
            for (auto it = node.begin(); it != node.end(); it++) {
                translation.insert(std::pair<std::string, entt::entity>(it->second["file"].as<std::string>(),
                                                                        it->second["id"].as<entt::entity>()));
            }
            setUp(registry);
        }

        void setUp(entt::registry *registry) {
            m_registry = registry;
        }

        bool run() {
            const std::filesystem::path path{"data/models"};
            ModelLoader* modelLoader = ModelLoader::createInstance(m_registry);
            for (const auto &entry: std::filesystem::directory_iterator(path)) {
                std::string fileName = entry.path().filename().string();
                bool found = fileName.find("example") != std::string::npos;
                if (fileName.find("example") != std::string::npos)
                    continue;
                for (auto it = translation.find(fileName); it != translation.end(); it = translation.find(fileName)) {
                    modelLoader->loadMesh(fileName);
                    translation.erase(it);
                }
            }
            return true;
        }

    private:
        entt::registry *m_registry;
        std::multimap<std::string, entt::entity> translation;
    };

    class GraphicsUnloader : public System {
    public:
        GraphicsUnloader() {
            flag = EngineStop;
            threadFlag = SingleThread;
        }

        void setUp(entt::registry *registry, YAML::Node &node) {
            setUp(registry);
        }

        void setUp(entt::registry *registry) {
            m_registry = registry;
        }

        bool run() {
            for (auto &&[item, program]: m_registry->view<Program>().each()) {
//                bgfx::destroy(program.programID);
            }

            for (auto &&[item, mesh]: m_registry->view<ModelComponent>().each()) {
//                meshUnload(mesh.mesh);
            }

            for (auto &&[item, vertex]: m_registry->view<VertexBuffer>().each()) {
//                bgfx::destroy(vertex.vbh);
            }

            for (auto &&[item, index]: m_registry->view<IndexBuffer>().each()) {
//                bgfx::destroy(index.ibh);
            }

            return true;
        }

    private:
        entt::registry *m_registry;
    };

    class GameTime : public System {
    public:
        GameTime() {
            flag = EngineRunning;
            lastTime = glfwGetTime();
        }

        void setUp(entt::registry *registry, YAML::Node &node) {
            entity = node["GameTime"]["timer"].as<entt::entity>();
            setUp(registry);
        }

        void setUp(entt::registry *registry) {
            m_registry = registry;
        }

        bool run() {
            auto &component = m_registry->get<Time>(entity);
            float currentFrame = glfwGetTime();
            float dt = currentFrame - component.lastFrame;
            component.dt = dt;
//            if (dt >= 1 / 60.f) {
            component.lastFrame = currentFrame;
//            }
            return true;
        }

    private:
        float lastTime = 0;
        int frames = 0;
        entt::registry *m_registry;
        entt::entity entity = entt::null;
    };

//    class CreateTimer : public System {
//    public:
//        CreateTimer() {
//            flag = EngineStart;
//        }
//
//        void setUp(entt::registry *registry, YAML::Node &node)  {
//            setUp(registry);
//        }
//
//        void setUp(entt::registry *registry)  {
//            m_registry = registry;
//        }
//
//        bool run()  {
//            entt::entity entity = m_registry->create();
//            m_registry->emplace_or_replace<Tag>(entity, "PhysicsClock");
//            m_registry->emplace_or_replace<Time>(entity);
//            m_registry->emplace_or_replace<Physics>(entity);
//            return true;
//        }
//
//    private:
//        entt::registry *m_registry;
//    };

    class Camera : public System {
    public:
        Camera() {
            threadFlag = SingleThread;
        }

        void setUp(entt::registry *registry, YAML::Node &node) {
            camera = node["Camera"]["camera"].as<entt::entity>();
            window = node["Camera"]["window"].as<entt::entity>();
            registry->get<WindowPtr>(window).sizeChange = true;
            setUp(registry);
        }

        void setUp(entt::registry *registry) {
            m_registry = registry;
            recalculatePosition(registry->get<CameraComponent>(camera).position);
        }

        bool run() {
            auto &windowComponent = m_registry->get<WindowPtr>(window);
            auto &transform = m_registry->get<Transform>(camera);
            auto &cameraComponent = m_registry->get<CameraComponent>(camera);
            auto attached = m_registry->get<AttachedTo>(camera).target;

            if (attached != entt::null) {
                transform.position = m_registry->get<Transform>(attached).position;
            }
            if (cameraComponent.position != transform.position) {
                cameraComponent.position = transform.position;
                recalculatePosition(cameraComponent.position);
            }

            if (cameraComponent.zoom != previousZoom) {
                scale = glm::identity<glm::mat4>();
                scale = glm::scale(scale, {cameraComponent.zoom, cameraComponent.zoom, cameraComponent.zoom});
//                bx::mtxScale(scale.data(), cameraComponent.zoom); //performs a zoom in on the camera.
                previousZoom = cameraComponent.zoom;
            }

            int width, height;
            if (windowComponent.sizeChange) {
                glfwGetWindowSize(windowComponent.window, &width, &height);
                float ratio = (float) width / (float) height;
                float half_height = height / 2.f;
                float half_width = half_height * ratio;

                ortho = glm::ortho(-half_width, half_width, -half_height, half_height, 0.0f, 100.f);
//                bx::mtxOrtho(ortho.data(), -half_width, half_width, -half_height, half_height, 0.0f, 100.f, 0.0f,
//                             bgfx::getCaps()->homogeneousDepth);
            }

            scaledOrtho = ortho * scale;
//            bx::mtxMul(scaledOrtho.data(), ortho.data(), scale.data());

//            bgfx::setViewTransform(0, glm::value_ptr(cameraMtx), scaledOrtho.data());

//            bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

            return true;
        }

        void recalculatePosition(glm::vec3 &cameraPos) {
            cameraMtx = glm::translate(glm::mat4(1.0), cameraPos) *
                        glm::rotate(glm::mat4(1.0), glm::radians(0.0f), glm::vec3(0.f, 0.f, 1.f));

            cameraMtx = glm::inverse(cameraMtx);
        }

    private:
        entt::entity camera = entt::null;
        entt::entity window = entt::null;
        entt::registry *m_registry;
        glm::mat4 ortho{0};
        glm::mat4 scaledOrtho{0};
        glm::mat4 scale{0};
        glm::mat4 cameraMtx;
        float previousZoom = 0.f;
    };

    class UpdateMovement : public System {
    public:

        UpdateMovement() {
        }

        void setUp(entt::registry *registry, YAML::Node &node) {
            timer = node["UpdateMovement"]["timer"].as<entt::entity>();
            setUp(registry);
        }

        void setUp(entt::registry *registry) {
            m_registry = registry;
        }

        bool run() {
            float dt = m_registry->get<Time>(timer).dt;
            if (dt >= 1 / 60.f) {
                std::cout << "jump " << dt << std::endl;

            }
            auto view = m_registry->view<Physics, Transform>();
            for (auto entity: view) {
                auto &transform = m_registry->get<Transform>(entity);
                auto &physics = m_registry->get<Physics>(entity);
                transform.position += (physics.velocity * dt) + 0.5f * physics.acceleration * dt * dt;
//                physics.velocity += physics.acceleration * dt;
//                transform.position += (physics.velocity * dt);
            }
//            }
            return true;
        }

    private:
        entt::registry *m_registry;
        entt::entity timer;
    };

    class PrimaryMovement : public System {
    public:
        PrimaryMovement() {
            input = std::make_unique<Input>(std::vector<int>{GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D},
                                            std::vector<int>{GLFW_MOUSE_BUTTON_MIDDLE});
        }

        void setUp(entt::registry *registry, YAML::Node &node) {
            camera = node["primaryMovement"]["focus"].as<entt::entity>();
            trackedObject = registry->get<AttachedTo>(camera).target;
            if (trackedObject == entt::null) {
                detached = true;
            }
            setUp(registry);
        }

        void setUp(entt::registry *registry) {
            m_registry = registry;
            observer.connect(*m_registry, entt::collector.update<PrimaryController>());
        }

        bool run() {
//            auto view = m_registry->view<PrimaryController>();
//            auto entity = view.front();
            if (!detached) {
                auto &physComp = m_registry->get<Physics>(trackedObject);
                if (input->keyIsDown() && !m_registry->any_of<MovementDisabled>(trackedObject)) {
                    physComp.velocity = {0.0f, 0.0f, 0.0f};
                    if (input->getIsKeyDown(GLFW_KEY_W)) {
                        physComp.velocity += glm::vec3(0, 1, 0);
                    }
                    if (input->getIsKeyDown(GLFW_KEY_S)) {
                        physComp.velocity -= glm::vec3(0, 1, 0);
                    }
                    if (input->getIsKeyDown(GLFW_KEY_A)) {
                        physComp.velocity -= glm::vec3(1, 0, 0);
                    }
                    if (input->getIsKeyDown(GLFW_KEY_D)) {
                        physComp.velocity += glm::vec3(1, 0, 0);
                    }
                    glm::normalize(physComp.velocity);
                    physComp.velocity *= 5;
                }
            } else {
                auto &physComp = m_registry->get<Physics>(camera);
                if (input->keyIsDown() && !m_registry->any_of<MovementDisabled>(camera)) {
                    physComp.velocity = {0.0f, 0.0f, 0.0f};
                    if (input->getIsKeyDown(GLFW_KEY_W)) {
                        physComp.velocity += glm::vec3(0, 1, 0);
                    }
                    if (input->getIsKeyDown(GLFW_KEY_S)) {
                        physComp.velocity -= glm::vec3(0, 1, 0);
                    }
                    if (input->getIsKeyDown(GLFW_KEY_A)) {
                        physComp.velocity -= glm::vec3(1, 0, 0);
                    }
                    if (input->getIsKeyDown(GLFW_KEY_D)) {
                        physComp.velocity += glm::vec3(1, 0, 0);
                    }
                    glm::normalize(physComp.velocity);
                    physComp.velocity *= 5;
                }
            }
//            auto &camComp = m_registry->get<CameraComponent>(camera);

            if (input->getIsMouseButtonDown(GLFW_MOUSE_BUTTON_MIDDLE) && detached) {
                if (input->xposUpdate) {
                    dx += input->getMouseOffset().first * 1;
                    input->xposUpdate = false;
                }
                if (input->yposUpdate) {
                    dy += input->getMouseOffset().second;
                    input->yposUpdate = false;
                }
            }

            auto scrollOffset = Input::getScroll();
            if (scrollOffset.second != previousScroll) {
                auto &zoom = m_registry->get<CameraComponent>(camera).zoom;
                zoom += scrollOffset.second;
                if (zoom < 0)
                    zoom = 0;
                previousScroll = scrollOffset.second;
            }

//            glm::vec3 dir;
            /*dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            dir.y = sin(glm::radians(pitch));
            dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));*/
//            dir.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
//            dir.y = sin(glm::radians(pitch));
//            dir.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
//            camComp.front = glm::normalize(dir);

//            camComp.right = glm::normalize(glm::cross({0, 1, 0}, camComp.front));
//            camComp.up = glm::normalize(glm::cross(camComp.front, camComp.right));

            return true;
        }

    private:
        std::unique_ptr<Input> input;
        float dx = 0;
        float dy = 0;

        entt::entity camera;
        entt::entity trackedObject;
        entt::observer observer;
        entt::registry *m_registry;

        double previousScroll = 0.;
        bool detached = false;
    };

    class Renderer : public System {
    public:
        Renderer() {
            threadFlag = SingleThread;
        }

        void setUp(entt::registry *registry, YAML::Node &node) {
            setUp(registry);
        }

        void setUp(entt::registry *registry) {
            m_registry = registry;
        }

        bool run() {
//            bgfx::touch(0);
//            {
//                auto view = m_registry->view<Transform, VertexBuffer, Program>();
//                for (auto &entity: view) {
//                    glm::mat4 transform = m_registry->get<Transform>(entity).getTransform();
//                    bgfx::setTransform(glm::value_ptr(transform));
//
//                    bgfx::setVertexBuffer(0, m_registry->get<VertexBuffer>(entity).vbh);
//                    auto index = m_registry->try_get<IndexBuffer>(entity);
//                    if (index) {
//                        bgfx::setIndexBuffer(index->ibh);
//                    }
//
//                    bgfx::setState(
//                            BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A);
//                    bgfx::submit(0, m_registry->get<Program>(entity).programID);
//                }
//            }
//
//            //render mesh
//            auto view = m_registry->view<Transform, ModelComponent, Program>();
//            for (auto &entity: view) {
//                meshSubmit(m_registry->get<ModelComponent>(entity).mesh, 0,
//                           m_registry->get<Program>(entity).programID,
//                           glm::value_ptr(m_registry->get<Transform>(entity).getTransform()), BGFX_STATE_DEFAULT);
//            }
//
//            bgfx::frame();
            return true;
        }

    private:
        entt::registry *m_registry;
    };

    class CloseEngine : public System {
    public:
        CloseEngine() {
            input = std::make_unique<Input>(std::vector<int>{GLFW_KEY_ESCAPE});
        }

        void setUp(entt::registry *registry, YAML::Node &node) {
            setUp(registry);
        }

        void setUp(entt::registry *registry) {
            m_registry = registry;
        }

        bool run() {
            if (input->getIsKeyDown(GLFW_KEY_ESCAPE)) {
                auto view = m_registry->view<WindowPtr>();
                for (auto &&[entity, comp]: view.each()) {
                    comp.running = false;
                }
            }
            return true;
        }

    private:
        std::unique_ptr<Input> input;
        entt::registry *m_registry;
    };


}


#endif //VULKAN_SYSTEM_H
