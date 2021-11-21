#ifndef VULKAN_MODELLOADER_H
#define VULKAN_MODELLOADER_H

#include <string>
#include <sstream>
#include <fstream>

#include "boxer/boxer.h"
#include "entt/entt.hpp"
#include "Entity.h"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "yaml-cpp/yaml.h"

namespace SGE {

    class ModelLoader {
    public:
        ModelLoader(entt::registry *registry);

        entt::entity loadMesh(const std::string &loc, entt::entity entity);

    private:
        ModelLoader() {
            this->m_registry = nullptr;
        }

        void handleModel(Entity &entity, YAML::Node& node);

        void handleMesh(Entity &entity, YAML::Node& node);

        entt::registry* m_registry;
    };







}


#endif //VULKAN_MODELLOADER_H
