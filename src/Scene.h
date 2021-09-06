#ifndef VULKAN_SCENE_H
#define VULKAN_SCENE_H

#include <entt/entt.hpp>
#include "Components.h"

namespace SGE {

    class Entity;

    class Scene {
    public:
        entt::registry m_world;

        Scene() = default;

        ~Scene() = default;

        Entity createEntity();

        void destroyEntity(Entity entity);

    private:


    };

}

#endif //VULKAN_SCENE_H
