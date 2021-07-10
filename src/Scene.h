#ifndef VULKAN_SCENE_H
#define VULKAN_SCENE_H

#include <entt/entt.hpp>
#include "Components.h"

namespace SGE {

    class Entity;

    class SystemManager;

    class Scene {
    public:
        Scene() = default;

        ~Scene() = default;

        Entity createEntity(const std::string &name);

        void destroyEntity(Entity entity);

    private:
        entt::registry m_world;

        //flecs::entity basePrefab = m_world.prefab("basePrefab").set<Transform>({{0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1}});
        //flecs::type base = m_world.type("base").add_instanceof(basePrefab).add<Transform>().add<Physics>();

        friend class Entity;

        friend class SystemManager;

    };

}

#endif //VULKAN_SCENE_H
