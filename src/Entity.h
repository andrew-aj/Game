#ifndef VULKAN_ENTITY_H
#define VULKAN_ENTITY_H

#include <entt/entt.hpp>

#include "Scene.h"

namespace SGE {

    class Entity {
    public:
        //Entity() {}

        Entity(entt::entity entity, Scene *scene) : m_entity(entity), m_scene(scene) {}

        explicit Entity(Scene *scene) : m_scene(scene) {
            m_entity = m_scene->m_world.create();
        }

        Entity(Scene *scene, const std::string &name) : m_scene(scene) {
            m_entity = m_scene->m_world.create();
            m_scene->m_world.emplace<Tag>(m_entity, name);
        }

        template<typename T, typename... Args>
        T& addComponent(Args... args) {
            return m_scene->m_world.emplace_or_replace<T>(m_entity, args...);
        }

        template<typename T>
        const T& getComponent() {
            return m_scene->m_world.try_get<T>(m_entity);
        }

        template<typename T>
        bool hasComponent() {
            return m_scene->m_world.all_of<T>(m_entity);
        }

        template<typename T>
        void setComponent(const T &&input) {
            m_scene->m_world.emplace_or_replace<T>(m_entity, std::forward<T>(input));
        }

        template<typename T>
        void removeComponent() {
            m_scene->m_world.remove<T>(m_entity);
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
            return m_entity == entity.m_entity && m_scene == entity.m_scene;
        }

        bool operator!=(const Entity &entity) {
            return m_entity != entity.m_entity && m_scene != entity.m_scene;
        }

    private:
        entt::entity m_entity = entt::null;
        Scene *m_scene = nullptr;
    };

}

#endif //VULKAN_ENTITY_H
