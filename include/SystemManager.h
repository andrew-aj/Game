

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
#include "Logger.h"
#include "Includes.h"

namespace SGE {

    class SystemManager {
    public:
        SystemManager(entt::registry &registry);

        ~SystemManager();

        SystemManager &operator=(SystemManager &&manager) noexcept;

        SystemManager(SystemManager &&manager);

        void startSystems();

        //template witchcraft
        template<typename T>
        std::future<bool> runSystemType(T *pointer);

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
                m_logger->writeToLog("Error, " + std::string(typeid(std::tuple_element<I, std::tuple<T...>>).name()) +
                                     "system did not return correct state.");
                return false;
            }
            return helper<p, I + 1>(t, array);
        }

        template<typename... T>
        bool runSystem(T *...pointers);

        bool runSystems();

        bool runStartUp();

        bool runShutDown();

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

        Logger *m_logger;
    };

}

#endif //VULKAN_SYSTEMMANAGER_H
