#ifndef VULKAN_MODELLOADER_H
#define VULKAN_MODELLOADER_H

#include <string>
#include <sstream>
#include <fstream>

#include "boxer/boxer.h"
#include "entt/entt.hpp"
#include "Entity.h"
#include "bgfxutils.h"

namespace SGE {

    class ModelLoader {
    public:
        ModelLoader(entt::registry *registry);

        entt::entity loadMesh(const std::string &loc);

    private:
        ModelLoader() {
            this->m_registry = nullptr;
        }

        void handleModel(Entity &entity, std::fstream &open);

        void handleMesh(Entity &entity, std::fstream &open);

        entt::registry* m_registry;
    };

    ModelLoader::ModelLoader(entt::registry *registry) {
        m_registry = registry;
    }

    entt::entity ModelLoader::loadMesh(const std::string &loc) {
        Entity result = Entity(m_registry);
        std::fstream open("data/models/" + loc, std::ios_base::in);

        if (open.fail()) {
            std::string message = "Error, cannot open and/or find the model for " + loc;
            boxer::show(message.c_str(), "Error!");
            return entt::null;
        }

        std::string line;

        //Set name
        std::getline(open, line);
        line = line.substr(1, line.length() - 2);
        result.addComponent<Tag>(line);

        std::getline(open, line);
        if (line.find("true") != std::string::npos) {
            handleModel(result, open);
        } else {
            handleMesh(result, open);
        }

        return (entt::entity) result;
    }

    void ModelLoader::handleModel(Entity &entity, std::fstream &open) {
        auto &model = entity.addComponent<ModelComponent>();

        std::string line;
        std::getline(open, line);
        line = line.substr(1, line.length() - 2);
        model.mesh = meshLoad(line.c_str());
    }

    void ModelLoader::handleMesh(Entity &entity, std::fstream &open) {
        auto &mesh = entity.addComponent<MeshComponent>();

        std::string line;
        std::getline(open, line);

        std::getline(open, line);
        line = line.substr(1, line.length() - 2);
        int verticesCount = std::stoi(line);
        mesh.vertices.reserve(verticesCount);

        std::getline(open, line);
        line = line.substr(1, line.length() - 2);
        int length = std::stoi(line);
        mesh.indices.reserve(length);

        std::getline(open, line, '}');
        auto &vertices = mesh.vertices;
        int count = 0;
        while (line.length() > 1) {
            int loc = line.find('{');
            while (loc >= 0) {
                line = line.substr(loc+1);
                loc = line.find('{');
            }

            Vertex vertex;

            std::stringstream stream;
            stream << line;

            std::getline(stream, line, ',');
            vertex.x = std::stof(line);

            std::getline(stream, line, ',');
            vertex.y = std::stof(line);

            std::getline(stream, line, '\0');
            vertex.z = std::stof(line);

            vertices.push_back(vertex);

            count++;
            if (count == verticesCount)
                break;

            std::getline(open, line, ',');
            std::getline(open, line, '}');
        }
        std::getline(open, line);

        auto &indices = mesh.indices;

        std::getline(open, line, '}');
        line = line.substr(2);
        std::stringstream stream;
        stream << line;
        for (int i = 0; i < length; i++) {
            std::getline(stream, line, ',');
            indices.push_back(std::stoi(line));
        }

        VertexBuffer &vbh = entity.addComponent<VertexBuffer>();
        bgfx::VertexLayout layout;
        layout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).end();
        vbh.vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size() * 12), layout);

        IndexBuffer &ibh = entity.addComponent<IndexBuffer>();
        ibh.ibh = bgfx::createIndexBuffer(bgfx::makeRef(indices.data(), indices.size() * 4));

        std::getline(open, line);
        std::getline(open, line);
        std::string vs = line.substr(1, line.length() - 2);

        std::getline(open, line);
        std::string fs = line.substr(1, line.length() - 2);

        Program &prgm = entity.addComponent<Program>();
        prgm.programID = loadProgram(vs.c_str(), fs.c_str());
    }

}


#endif //VULKAN_MODELLOADER_H
