

#ifndef VULKAN_SYSTEMMANAGER_H
#define VULKAN_SYSTEMMANAGER_H

#include <thread>
#include <map>
#include <cassert>
#include <future>
#include <vector>
#include <string>
#include <functional>
#include <numeric>
#include "System.h"
#include "Scene.h"
#include "Logger.h"
#include "Includes.h"

namespace SGE {

    class SystemManager {
    public:
        static constexpr unsigned short min_priority = std::numeric_limits<unsigned short>::max();
        static constexpr unsigned short max_priority = std::numeric_limits<unsigned short>::min();

        explicit SystemManager(Scene &scene) {
            m_world = &scene.m_world;
            logger = Logger::getInstance();
        }

        ~SystemManager() {
        }

        SystemManager &operator=(SystemManager &&manager) noexcept {
            m_world = manager.m_world;
            manager.m_world = nullptr;

            return *this;
        }

        SystemManager(SystemManager &&manager) {
            m_world = manager.m_world;
            manager.m_world = nullptr;
        }

        void startSystems() {
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
        std::future<bool> runSystemType(T *pointer) {
            auto policy = convertPolicy(pointer->threadFlag);
            static_assert(std::is_member_function_pointer<decltype(&T::run)>::value,
                          "Failed to find member function bool run().");
            return std::async(policy, &T::run, pointer);
        }

        template<std::size_t p, std::size_t I = 0, typename... T>
        inline typename std::enable_if<I == sizeof...(T), bool>::type
        helper(std::tuple<T...> &t, std::array<std::future<bool>, p> &array) {
            return true;
        }

        template<std::size_t p, std::size_t I = 0, typename... T>
        inline typename std::enable_if<I < sizeof...(T), bool>::type
        helper(std::tuple<T...> &t, std::array<std::future<bool>, p> &array) {
            auto &out = array[I];
            if (!out.get()) {
                logger->writeToLog("Error, " + std::string(typeid(std::tuple_element<I, std::tuple<T...>>).name()) +
                                   "system did not return correct state.");
                return false;
            }
            return helper<p, I + 1>(t, array);
        }

        template<typename... T>
        bool runSystem(T *...pointers) {
            std::tuple<T...> t;
            std::array<std::future<bool>, sizeof...(pointers)> array = {runSystemType(pointers)...};
            return helper<array.size()>(t, array);
        }

        std::launch convertPolicy(ThreadFlag flag) {
            return arr[flag];
        }

        bool runSystems() {
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

        bool runStartUp() {
            return meshModelLoader->run();
        }

        bool runShutDown() {
            graphicsUnloader->run();
            return true;
        }

    private:
        //always running systems
        std::unique_ptr<GameTime> gameTime = std::make_unique<GameTime>();
        std::unique_ptr<PrimaryMovement> primaryMovement = std::make_unique<PrimaryMovement>();
        std::unique_ptr<UpdateMovement> updateMovement = std::make_unique<UpdateMovement>();
        std::unique_ptr<Camera> camera = std::make_unique<Camera>();
        std::unique_ptr<Renderer> render = std::make_unique<Renderer>();

        //startup systems
        std::unique_ptr<MeshModelLoader> meshModelLoader = std::make_unique<MeshModelLoader>();

        //shutdown systems
        std::unique_ptr<CloseEngine> closeEngine = std::make_unique<CloseEngine>();
        std::unique_ptr<GraphicsUnloader> graphicsUnloader = std::make_unique<GraphicsUnloader>();

        //store conversion array for quick access;
        std::launch arr[2]{std::launch::async, std::launch::deferred};

        entt::registry *m_world;

        Logger *logger;
    };

}

#endif //VULKAN_SYSTEMMANAGER_H
