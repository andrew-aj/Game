#ifndef VULKAN_SYSTEM_H
#define VULKAN_SYSTEM_H

#include <entt/entt.hpp>
#include <cassert>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <execution>

#include "ModelLoader.h"
#include "Input.h"

namespace SGE {

    enum SystemFlag {
        EngineStart,
        EngineRunning,
        EngineStop
    };

    class System {
    public:
        virtual bool run(entt::registry *m_world) = 0;

        SystemFlag flag = EngineRunning;
    };

    void windowSizeUpdate(entt::registry &m_world, int width, int height) {
        for (auto &&[item, ui]: m_world.view<UIComponent>().each()) {
            if (ui.scalingFunction) {
                ui.scalingFunction(ui.xTop, ui.yTop, ui.xBottom, ui.yBottom, ui.xScale, ui.yScale, width, height);
            }
        }
    }

    class MeshModelLoader : public System {
    public:
        MeshModelLoader() {
            flag = EngineStart;
        }

        bool run(entt::registry *m_world) override {
            const std::filesystem::path path{"data/models"};
            ModelLoader modelLoader(m_world);
            for (const auto &entry: std::filesystem::directory_iterator(path)) {
                std::string fileName = entry.path().filename().string();
                if (fileName.find("example"))
                    continue;
                modelLoader.loadMesh(fileName);
            }
            return true;
        }
    };

    class GraphicsUnloader : public System {
    public:
        GraphicsUnloader() {
            flag = EngineStop;
        }

        bool run(entt::registry *m_world) override {
            for (auto &&[item, program]: m_world->view<Program>().each()) {
                bgfx::destroy(program.programID);
            }

            for (auto &&[item, mesh]: m_world->view<ModelComponent>().each()) {
                meshUnload(mesh.mesh);
            }

            for (auto &&[item, vertex]: m_world->view<VertexBuffer>().each()) {
                bgfx::destroy(vertex.vbh);
            }

            for (auto &&[item, index]: m_world->view<IndexBuffer>().each()) {
                bgfx::destroy(index.ibh);
            }

            return true;
        }
    };

    class GameTime : public System {
    public:
        GameTime() {
            flag = EngineRunning;
        }

        bool run(entt::registry *m_world) override {
            auto view = m_world->view<Time, Tag, Physics>();
            for (auto &entity: view) {
                if (m_world->get<Tag>(entity).name == "PhysicsClock") {
                    auto &component = m_world->get<Time>(entity);
                    int64_t freq = bx::getHPFrequency();
                    int64_t frameTime = bx::getHPFrequency() - component.time;
                    component.dtimeNS = frameTime * 1000000 / freq;
                    component.time = bx::getHPCounter();
                }
            }
            return true;
        }
    };

    class CreateTimer : public System {
    public:
        CreateTimer() {
            flag = EngineStart;
        }

        bool run(entt::registry *m_world) override {
            entt::entity entity = m_world->create();
            m_world->emplace_or_replace<Tag>(entity, "PhysicsClock");
            m_world->emplace_or_replace<Time>(entity);
            m_world->emplace_or_replace<Physics>(entity);
            return true;
        }
    };

    class Camera : public System {
    public:

        bool run(entt::registry *m_world) override {
            if (camera == entt::null) {
                auto &comp = m_world->emplace_or_replace<CameraComponent>(camera);
                auto &comp2 = m_world->emplace_or_replace<Transform>(camera);
                m_world->emplace_or_replace<PrimaryController>(camera);
                m_world->emplace_or_replace<Physics>(camera);
                comp.position = {0.f, 0.f, 0.f};
                comp2.position = comp.position;
                comp.front = {0, 0, -1.f};
                comp.up = {0, 1, 0};
            }
            auto view = m_world->view<WindowPtr>();
            entt::entity window = view.front();
            auto view2 = m_world->view<CameraComponent, Transform>();
            entt::entity c = view2.front();
            auto &comp = m_world->get<WindowPtr>(window);
            auto &transform = m_world->get<Transform>(c);
            auto &camComp = m_world->get<CameraComponent>(c);
            camComp.position = transform.position;
            float proj[16];
            int width, height;
            glfwGetWindowSize(comp.window, &width, &height);
            bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            auto mat = glm::lookAt(camComp.position, camComp.position + camComp.front, camComp.up);
            bgfx::setViewTransform(0, glm::value_ptr(mat), proj);

            bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
            return true;
        }

    private:
        entt::entity camera = entt::null;
    };

    class UpdateMovement : public System {
    public:
        bool run(entt::registry *m_world) override {
            int64_t dt = 0;
            {
                auto view = m_world->view<Time, Tag>();
                for (auto &entity: view) {
                    if (m_world->try_get<Tag>(entity)->name == "PhysicsClock") {
                        dt = m_world->try_get<Time>(entity)->dtimeNS;
                    }
                }
            }
            auto view = m_world->view<Physics, Transform>();
            for (auto entity: view) {
                auto &transform = m_world->get<Transform>(entity);
                auto &physics = m_world->get<Physics>(entity);
                transform.position += physics.velocity.operator*=(dt);
            }
            return true;
        }
    };

    class primaryMovement : public System {
    public:
        primaryMovement() {
            input = new Input({GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D}, {});
        }

        ~primaryMovement() {
            delete input;
        }

        bool run(entt::registry *m_world) override {
            auto view = m_world->view<PrimaryController>();
            auto entity = view.front();
            auto &physComp = m_world->get<Physics>(entity);
            auto &camComp = m_world->get<CameraComponent>(entity);

            yaw += input->getMouseOffset().first;
            pitch += input->getMouseOffset().second;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 dir;
            dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            dir.y = sin(glm::radians(pitch));
            dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            camComp.front = glm::normalize(dir);

            camComp.right = glm::normalize(glm::cross(camComp.front, {0, 1, 0}));
            camComp.up = glm::normalize(glm::cross(camComp.right, camComp.front));

            physComp.velocity = {0, 0, 0};

            if (input->getIsKeyDown(GLFW_KEY_W)) {
                physComp.velocity += camComp.front;
            }
            if (input->getIsKeyDown(GLFW_KEY_S)) {
                physComp.velocity += camComp.front;
            }
            if (input->getIsKeyDown(GLFW_KEY_A)) {
                physComp.velocity -= camComp.right;
            }
            if (input->getIsKeyDown(GLFW_KEY_D)) {
                physComp.velocity += camComp.right;
            }
            glm::normalize(physComp.velocity);
            physComp.velocity *= 5;
            return true;
        }

    private:
        Input *input;
        float yaw = 0;
        float pitch = 0;
    };

    class Renderer : public System {
    public:
        bool run(entt::registry *m_world) override {
            bgfx::touch(0);
            {
                auto view = m_world->view<Transform, VertexBuffer, Program>();
                for (auto &entity: view) {
                    glm::mat4 transform = m_world->get<Transform>(entity).getTransform();
                    bgfx::setTransform(glm::value_ptr(transform));

                    bgfx::setVertexBuffer(0, m_world->get<VertexBuffer>(entity).vbh);
                    auto index = m_world->try_get<IndexBuffer>(entity);
                    if (index) {
                        bgfx::setIndexBuffer(index->ibh);
                    }

                    bgfx::setState(BGFX_STATE_DEFAULT);

                    bgfx::submit(0, m_world->get<Program>(entity).programID);
                }
            }

            //render mesh
            auto view = m_world->view<Transform, ModelComponent, Program>();
            for (auto &entity: view) {
                meshSubmit(m_world->get<ModelComponent>(entity).mesh, 0, m_world->get<Program>(entity).programID,
                           glm::value_ptr(m_world->get<Transform>(entity).getTransform()), BGFX_STATE_DEFAULT);
            }

            bgfx::frame();
            return true;
        }
    };


}


#endif //VULKAN_SYSTEM_H
