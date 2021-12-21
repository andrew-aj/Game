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
#include "DeviceClass.h"
#include "yaml-cpp/yaml.h"

namespace SGE {
    class ModelLoader {
    public:
        static ModelLoader *createInstance(entt::registry *registry);

        static ModelLoader *createInstance();

        bool loadMesh(const std::string &loc);

        bool loadVertexBuffer(const std::string &loc);

        bool loadIndexBuffer(const std::string &loc);

        bool loadShader(const std::string& loc);

        ModelLoader(entt::registry *registry);

        MeshComponent& getMesh(uint32_t id);

        VertexBuffer getVertexBuffer(uint32_t id);

        IndexBuffer getIndexBuffer(uint32_t id);

        Program getShaderProgram(uint32_t id);

        ModelViewProjMatrix getProjectionMatrix(uint32_t id);

    private:
        ModelLoader() {
            this->m_registry = nullptr;
        }

        static entt::registry *m_registry;

        static std::unique_ptr<ModelLoader> modelLoader;

        std::unordered_map<uint32_t, MeshComponent> meshStorage;
        std::unordered_map<uint32_t, Diligent::RefCntAutoPtr<Diligent::IBuffer>> vbStorage;
        std::unordered_map<uint32_t, Diligent::RefCntAutoPtr<Diligent::IBuffer>> ibStorage;
        std::unordered_map<uint32_t, std::pair<Diligent::RefCntAutoPtr<Diligent::IPipelineState>, Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding>>> programStorage;
        std::unordered_map<uint32_t, Diligent::RefCntAutoPtr<Diligent::IBuffer>> projMatrixStorage;

    };


}


#endif //VULKAN_MODELLOADER_H
