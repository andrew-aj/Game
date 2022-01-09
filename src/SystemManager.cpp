#include "SystemManager.h"

namespace SGE {
    SystemManager::SystemManager(entt::registry &registry) {
        m_world = &registry;
        m_logger = Logger::getInstance();
    }

    SystemManager::~SystemManager() {
    }

    SystemManager &SystemManager::operator=(SystemManager &&manager) noexcept {
        m_world = manager.m_world;
        manager.m_world = nullptr;

        return *this;
    }

    SystemManager::SystemManager(SystemManager &&manager) {
        m_world = manager.m_world;
        manager.m_world = nullptr;
    }

    void SystemManager::startSystems() {
        YAML::Node node = YAML::LoadFile("data/systems/systems.yml");
        node = node["Systems"];
        gameTime->setUp(m_world, node);
        primaryMovement->setUp(m_world, node);
        updateMovement->setUp(m_world, node);
        camera->setUp(m_world, node);
        render->setUp(m_world, node);
        meshModelLoader->setUp(m_world, node);
        closeEngine->setUp(m_world, node);
        graphicsUnloader->setUp(m_world, node);
    }

//template witchcraft
    template<typename T>
    std::future<bool> SystemManager::runSystemType(T *pointer) {
        auto policy = arr[pointer->threadFlag];
        static_assert(std::is_member_function_pointer<decltype(&T::run)>::value,
                      "Failed to find member function bool run().");
        return std::async(policy, &T::run, pointer);
    }

    template<typename... T>
    bool SystemManager::runSystem(T *...pointers) {
        std::tuple<T...> t;
        std::array<std::future<bool>, sizeof...(pointers)> array = {runSystemType(pointers)...};
        return helper<array.size()>(t, array);
    }

    bool SystemManager::runSystems() {
        //first set of systems
        if (!runSystem(gameTime.get(), closeEngine.get())) {
            return false;
        }

        //second set of systems
        if (!runSystem(primaryMovement.get())) {
            return false;
        }

        //third set of systems
        if (!runSystem(updateMovement.get())) {
            return false;
        }

        //fourth set of systems
        if (!runSystem(camera.get())) {
            return false;
        }

        //fifth set of systems
        if (!runSystem(render.get())) {
            return false;
        }

        return true;
    }

    bool SystemManager::runStartUp() {
        return meshModelLoader->run();
    }

    bool SystemManager::runShutDown() {
        graphicsUnloader->run();
        closeEngine->run();
        return true;
    }
}