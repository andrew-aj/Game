#ifndef VULKAN_SYSTEM_H
#define VULKAN_SYSTEM_H

#include <entt/entt.hpp>
#include <cassert>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

#include "ModelLoader.h"

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
        for (auto &&[item, ui] : m_world.view<UIComponent>().each()) {
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
            for (const auto &entry : std::filesystem::directory_iterator(path)) {
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
            for (auto &&[item, program] : m_world->view<Program>().each()) {
                bgfx::destroy(program.programID);
            }

            for (auto &&[item, mesh] : m_world->view<ModelComponent>().each()) {
                meshUnload(mesh.mesh);
            }

            for (auto &&[item, vertex] : m_world->view<VertexBuffer>().each()) {
                bgfx::destroy(vertex.vbh);
            }

            for (auto &&[item, index] : m_world->view<IndexBuffer>().each()) {
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
            auto view = m_world->view<Time, Tag>();
            for (auto &entity : view) {
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
            return true;
        }
    };

    class Camera : public System {
    public:

        bool run(entt::registry *m_world) override {
            if (camera == entt::null){
                m_world->emplace_or_replace<CameraComponent>(camera);
                auto& comp = m_world->emplace_or_replace<Transform>(camera);
                comp.position = {0.f, 0.f, 0.f};
                comp.rotation = glm::quatLookAt(glm::vec3(0,0,0), glm::vec3(0, 1, 0));
                comp.scale = {1, 1, 1};
            }
            auto view = m_world->view<WindowPtr>();
            entt::entity window = view.front();
            auto view2 = m_world->view<CameraComponent, Transform>();
            entt::entity c = view2.front();
            auto& comp = m_world->get<WindowPtr>(window);
            auto& transform = m_world->get<Transform>(c);
            float proj[16];
            int width, height;
            glfwGetWindowSize(comp.window, &width, &height);
            bx::mtxProj(proj, 60.0f, float(width)/float(height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, glm::value_ptr(transform.getTransform()), proj);

            bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
            return true;
        }

    private:
        entt::entity camera = entt::null;
    };
}


#endif //VULKAN_SYSTEM_H
