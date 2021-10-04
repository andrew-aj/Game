#ifndef VULKAN_ENTITY_H
#define VULKAN_ENTITY_H

#include <entt/entt.hpp>

#include "Scene.h"

namespace SGE {

    class Entity {
    public:
        //Entity() {}

        Entity(entt::entity entity, Scene *scene) : m_entity(entity), m_world(&scene->m_world) {}

        explicit Entity(Scene *scene) : m_world(&scene->m_world) {
            m_entity = m_world->create();
        }

        Entity(entt::entity entity, entt::registry *m_registry) : m_entity(entity), m_world(m_registry) {}

        explicit Entity(entt::registry *m_registry) : m_world(m_registry) {
            m_entity = m_world->create();
        }

        Entity(Scene *scene, const std::string &name) : m_world(&scene->m_world) {
            m_entity = m_world->create();
            m_world->emplace<Tag>(m_entity, name);
        }

        template<typename T, typename... Args>
        T &addComponent(Args... args) {
            return m_world->emplace_or_replace<T>(m_entity, args...);
        }

        template<typename T>
        const T &getComponent() {
            return m_world->try_get<T>(m_entity);
        }

        template<typename T>
        bool hasComponent() {
            return m_world->all_of<T>(m_entity);
        }

        template<typename T>
        void setComponent(const T &&input) {
            m_world->emplace_or_replace<T>(m_entity, std::forward<T>(input));
        }

        template<typename T>
        void removeComponent() {
            m_world->remove<T>(m_entity);
        }

        explicit operator bool() {
            return m_entity != entt::null;
        }

        explicit operator entt::entity() const {
            return m_entity;
        }

        explicit operator uint32_t() const {
            return (uint32_t) m_entity;
        }

        bool operator==(const Entity &entity) {
            return m_entity == entity.m_entity && m_world == entity.m_world;
        }

        bool operator!=(const Entity &entity) {
            return m_entity != entity.m_entity && m_world != entity.m_world;
        }

    private:
        entt::entity m_entity = entt::null;
        entt::registry *m_world = nullptr;
    };

}

#endif //VULKAN_ENTITY_H
