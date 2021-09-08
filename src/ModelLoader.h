#ifndef VULKAN_MODELLOADER_H
#define VULKAN_MODELLOADER_H

#include <string>
#include <fstream>

#include "boxer/boxer.h"
#include "entt/entt.hpp"
#include "Scene.h"
#include "Entity.h"

namespace SGE {

    class ModelLoader {
    public:
        ModelLoader(Scene *scene);

        entt::entity loadMesh(const std::string &loc);

    private:
        ModelLoader() : ModelLoader(nullptr) {}

        Scene *scene;
    };

    ModelLoader::ModelLoader(Scene *scene) {
        this->scene = scene;
    }

    entt::entity ModelLoader::loadMesh(const std::string &loc) {
        Entity result = scene->createEntity();
        std::fstream open("data/models/" + loc, std::ios_base::in);

        if (open.fail()) {
            std::string message = "Error, cannot open and/or find the model for " + loc;
            boxer::show(message.c_str(), "Error!");
            return entt::null;
        }

        if (loc.find("Model") != std::string::npos){
            std::string line;
            std::getline(open, line);

        }
    }

}


#endif //VULKAN_MODELLOADER_H
