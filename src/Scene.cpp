#include "Scene.h"
#include "Entity.h"

namespace SGE{
    Entity Scene::createEntity(const std::string &name) {
        Entity entity(m_world.create(), this);
        return entity;
    }

    void Scene::destroyEntity(Entity entity) {
        m_world.destroy((entt::entity) entity);
    }
}

