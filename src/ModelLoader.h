#ifndef VULKAN_MODELLOADER_H
#define VULKAN_MODELLOADER_H

#include <string>
#include <fstream>

#include "boxer/boxer.h"

class ModelLoader {
public:
    bool success = true;

    ModelLoader(const std::string& path);
private:
    ModelLoader() : ModelLoader("example.dt") {}
};

ModelLoader::ModelLoader(const std::string &path) {
    std::fstream loader("data/models/"+path, std::ios_base::in);

    if (loader.fail()){
        boxer::show("Model does not exist.", "Error");
        success = false;
        return;
    }


}


#endif //VULKAN_MODELLOADER_H
