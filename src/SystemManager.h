

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

namespace SGE {

    class SystemManager {
    public:
        static constexpr unsigned short min_priority = std::numeric_limits<unsigned short>::max();
        static constexpr unsigned short max_priority = std::numeric_limits<unsigned short>::min();

        explicit SystemManager(Scene& scene) {
            unsigned int temp = std::thread::hardware_concurrency();
            if (temp > 1 && temp <= 64) {
                MAX_THREADS = std::thread::hardware_concurrency();
            }
            m_world = &scene.m_world;
            //threadStorage = new std::thread[MAX_THREADS];
        }

        ~SystemManager() {
            for (auto &it : priorityQueue) {
                delete it.second;
            }
        }

        SystemManager &operator=(SystemManager &&manager) noexcept {
            MAX_THREADS = manager.MAX_THREADS;

            priorityQueue = std::move(manager.priorityQueue);

            m_world = manager.m_world;
            manager.m_world = nullptr;

            //delete[] threadStorage;
            //threadStorage = manager.threadStorage;

            //manager.threadStorage = nullptr;

            return *this;
        }

        SystemManager(SystemManager &&manager) {
            MAX_THREADS = manager.MAX_THREADS;

            priorityQueue = std::move(manager.priorityQueue);

            m_world = manager.m_world;
            manager.m_world = nullptr;

            //delete[] threadStorage;
            //threadStorage = manager.threadStorage;

            //manager.threadStorage = nullptr;
        }

        void registerSystem(System *system, unsigned short priority) {
            priorityQueue.insert(std::make_pair(priority, system));
        }
/*
        template<typename out, typename... Args>
        void createInitialSystem(std::function<out(Args...)> func, Args... args){
            func(args...);
        }

        template<typename... Args>
        void createInitialSystemVoid(std::function<void(Args...)> func, Args... args){
            func(args...);
        }*/

        bool tickSystem(unsigned short priority) {
            assert(priorityQueue.find(priority) != priorityQueue.end());
            auto systems = priorityQueue.equal_range(priority);
            std::vector<std::future<bool>> asyncStorage;
            for (auto it = systems.first; it != systems.second; it++) {
                std::future<bool> a = std::async(std::launch::async, &System::run, it->second, m_world);
                asyncStorage.push_back(std::move(a));
            }
            for (int i = 0; i < asyncStorage.size(); i++) {
                if (!asyncStorage[i].get()) {
                    throw std::runtime_error("Error, system in " + std::to_string(priority) + " priority and index " +
                                             std::to_string(i) + " failed.");
                }
            }
            return true;
        }

        bool tickSystem() {
            return tickSystem(currentVal++);
        }

        bool canTick(){
            if (priorityQueue.find(currentVal) == priorityQueue.end()) {
                currentVal = 0;
                return false;
            }
            return true;
        }

    private:
        unsigned short MAX_THREADS = 1;

        //Stores both the System and the associated priority value with it
        //0 is highest priority
        std::multimap<unsigned short, System *> priorityQueue;

        entt::registry *m_world;

        //std::thread *threadStorage;

        unsigned short currentVal = 0;
    };

}

#endif //VULKAN_SYSTEMMANAGER_H
