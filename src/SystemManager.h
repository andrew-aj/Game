

#ifndef VULKAN_SYSTEMMANAGER_H
#define VULKAN_SYSTEMMANAGER_H

#include <thread>
#include <map>
#include <cassert>
#include <future>
#include <vector>
#include <entt/entt.hpp>
#include <string>
#include <functional>
#include <numeric>
#include "System.h"
#include "Scene.h"
#include "Logger.h"

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
//            for (auto &it: priorityQueue) {
//                delete it.second;
//            }
//            for (auto &it: startUpQueue) {
//                delete it.second;
//            }
//            for (auto &it: shutDownQueue) {
//                delete it.second;
//            }
        }

        SystemManager &operator=(SystemManager &&manager) noexcept {
//            MAX_THREADS = manager.MAX_THREADS;

//            priorityQueue = std::move(manager.priorityQueue);

            m_world = manager.m_world;
            manager.m_world = nullptr;

            //delete[] threadStorage;
            //threadStorage = manager.threadStorage;

            //manager.threadStorage = nullptr;

            return *this;
        }

        SystemManager(SystemManager &&manager) {
//            MAX_THREADS = manager.MAX_THREADS;

//            priorityQueue = std::move(manager.priorityQueue);

            m_world = manager.m_world;
            manager.m_world = nullptr;

            //delete[] threadStorage;
            //threadStorage = manager.threadStorage;

            //manager.threadStorage = nullptr;
        }

//        void registerSystem(System *system, unsigned short priority) {
//            YAML::Node node = YAML::LoadFile("data/systems/systems.yml");
//            node = node["Systems"];
//            system->setUp(m_world, node);
//            switch (system->flag) {
//                case EngineStart:
//                    startUpQueue.insert(std::make_pair(priority, system));
//                    break;
//                case EngineRunning:
//                    priorityQueue.insert(std::make_pair(priority, system));
//                    break;
//                case EngineStop:
//                    shutDownQueue.insert(std::make_pair(priority, system));
//                    break;
//                default:
//                    break;
//            }
//        }

/*
        template<typename out, typename... Args>
        void createInitialSystem(std::function<out(Args...)> func, Args... args){
            func(args...);
        }

        template<typename... Args>
        void createInitialSystemVoid(std::function<void(Args...)> func, Args... args){
            func(args...);
        }*/

        template<typename T>
        std::future<bool> runSystemType(T *pointer) {
            auto policy = convertPolicy(pointer->threadFlag);
            static_assert(std::is_member_function_pointer<decltype(&T::run)>::value,
                          "Failed to find member function bool run().");
            return std::async(policy, &T::run, pointer);
        }

        template<std::size_t p, std::size_t I = 0, typename... T>
        inline typename std::enable_if<I == sizeof...(T), bool>::type
        helper(std::tuple<T...>&t, std::array<std::future<bool>, p>& array) {
            return true;
        }

        template<std::size_t p, std::size_t I = 0, typename... T>
        inline typename std::enable_if<I < sizeof...(T), bool>::type
        helper(std::tuple<T...>& t, std::array<std::future<bool>, p>& array) {
            auto &out = array[I];
            if (!out.get()) {
                logger->writeToLog("Error, " + std::string(typeid(std::tuple_element<I, std::tuple<T...>>).name()) +
                                   "system did not return correct state.");
                return false;
            }
            return helper<p, I+1>(t, array);
        }

        template<typename... T>
        bool runSystem(T *...pointers) {
            std::tuple<T...> t;
            std::array<std::future<bool>, sizeof...(pointers)> array = {runSystemType(pointers...)};
            return helper<array.size()>(t, array);
//            for (int i = 0; i < array.size(); i++) {
//                auto &out = array[i];
//                if (!out.get()) {
//                    logger->writeToLog(
//                            ("Error," + typeid().name()
//                    " system did not return correct state."));
//                    return false;
//                }
//            }
//            return true;
        }

        std::launch convertPolicy(ThreadFlag flag) {
            return arr[flag];
        }

        bool runSystems() {
            //first set of systems
            if (!runSystem(gameTime.get())) {
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


//            auto iterator = priorityQueue.begin();
//            while (iterator != priorityQueue.end()) {
//                auto first = iterator->first;
//                iterator++;
//                auto second = iterator->first;
//                while (first == second) {
//                    iterator++;
//                    first = second;
//                    second = iterator->first;
//                }
//                iterator--;
//
//                auto systems = priorityQueue.equal_range(iterator->first);
//                std::vector<std::future<bool>> asyncStorage;
//                for (auto it = systems.first; it != systems.second; it++) {
//                    std::future<bool> a;
//                    auto policy = (it->second->threadFlag == SingleThread) ? std::launch::deferred
//                                                                           : std::launch::async;
//                    a = std::async(policy, &System::run)
//                    switch (it->second->type) {
//                        case SystemType::MeshModelLoader:
//                            a = std::async(policy, &System::run, (MeshModelLoader *) it->second);
//                            break;
//                        case SystemType::GraphicsUnloader:
//                            a = std::async(policy, &System::run, (GraphicsUnloader *) it->second);
//                            break;
//                        case SystemType::GameTime:
//                            a = std::async(policy, &System::run, (GameTime *) it->second);
//                            break;
//                        case SystemType::Camera:
//                            a = std::async(policy, &System::run, (Camera *) it->second);
//                            break;
//                        case SystemType::UpdateMovement:
//                            a = std::async(policy, &System::run, (UpdateMovement *) it->second);
//                            break;
//                        case SystemType::PrimaryMovement:
//                            a = std::async(policy, &System::run, (PrimaryMovement *) it->second);
//                            break;
//                        case SystemType::Renderer:
//                            a = std::async(policy, &System::run, (Renderer *) it->second);
//                            break;
//                        case SystemType::CloseEngine:
//                            a = std::async(policy, &System::run, (CloseEngine *) it->second);
//                            break;
//                        default:
//                            a = std::async(policy, &System::run, (System *) it->second);
//                            break;
//                    }
//                    asyncStorage.push_back(std::move(a));
//                }
//                for (int i = 0; i < asyncStorage.size(); i++) {
//                    if (!asyncStorage[i].get()) {
//                        boxer::show(("Error, system in " + std::to_string(iterator->first) + " priority and index " +
//                                     std::to_string(i) + " failed.").c_str(), "System Error");
//                        return false;
//                    }
//                }
//
//                iterator++;
//            }
            return true;
        }

        bool runStartUp() {
            return meshModelLoader->run();
        }

        bool runShutDown() {
            closeEngine->run();
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
