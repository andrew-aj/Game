#include "ModelLoader.h"
#include "Engine.h"

namespace SGE {

    ModelLoader::ModelLoader(entt::registry *registry) {
        m_registry = registry;
    }

    entt::entity ModelLoader::loadMesh(const std::string &loc, entt::entity entity) {
        Entity result = Entity(entity, m_registry);
//        std::fstream open("data/models/" + loc, std::ios_base::in);
        YAML::Node node;
        try {
            node = YAML::LoadFile("data/models" + loc);
        } catch (YAML::ParserException &e) {
            std::cout << e.what() << std::endl;
            return entt::null;
        }


        //Set name
        result.addComponent<Tag>(node["Name"].as<std::string>());

        if (node["Model"].as<bool>()) {
            handleModel(result, node);
        } else {
            handleMesh(result, node);
        }

        return (entt::entity) result;
    }

    void ModelLoader::handleModel(Entity &entity, YAML::Node &node) {
        auto &model = entity.addComponent<ModelComponent>();

//        model.mesh = meshLoad(line.c_str());

//        std::getline(open, line);
//        line = line.substr(2, line.length()-4);
//        std::stringstream stringstream;
//        stringstream << line;
//        std::getline(stringstream, line, ',');
//        float x = std::stof(line);

//        std::getline(stringstream, line, ',');
//        float y = std::stof(line);

//        std::getline(stringstream, line);
//        float z = std::stof(line);

//        entity.addComponent<Transform>().position = {x, y, z};
    }

    void ModelLoader::handleMesh(Entity &entity, YAML::Node &node) {
        auto &mesh = entity.addComponent<MeshComponent>();

        int verticesCount = node["NumVerts"].as<int>();
        mesh.vertices.reserve(verticesCount);

        int length = node["NumIndices"].as<int>();
        mesh.indices.reserve(length);

        mesh.vertices = node["Vertices"].as<std::vector<Vertex>>();

        mesh.indices = node["Indices"].as<std::vector<unsigned short>>();

        VertexBuffer &vbh = entity.addComponent<VertexBuffer>();
        Diligent::BufferDesc VertBuffDesc;
        VertBuffDesc.Name = entity.getComponent<Tag>().name.c_str();
        VertBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
        VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
        VertBuffDesc.Size = mesh.vertices.size() * sizeof(Vertex);
        Diligent::BufferData VBData;
        VBData.pData = mesh.vertices.data();
        VBData.DataSize = mesh.vertices.size() * sizeof(Vertex);
        Engine::getDevice()->CreateBuffer(VertBuffDesc, &VBData, &vbh.vertexBuffer);

        IndexBuffer &ibh = entity.addComponent<IndexBuffer>();
        Diligent::BufferDesc IndBuffDesc;
        IndBuffDesc.Name = entity.getComponent<Tag>().name.c_str();
        IndBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
        IndBuffDesc.BindFlags = Diligent::BIND_INDEX_BUFFER;
        IndBuffDesc.Size = mesh.indices.size() * sizeof(short);
        Diligent::BufferData IBData;
        IBData.pData = mesh.indices.data();
        IBData.DataSize = mesh.indices.size() * sizeof(short);
        Engine::getDevice()->CreateBuffer(IndBuffDesc, &IBData, &ibh.indexBuffer);

        std::string vs = node["VertexShader"].as<std::string>();

        std::string fs = node["FragmentShader"].as<std::string>();

        Program &prgm = entity.addComponent<Program>();

        Diligent::GraphicsPipelineStateCreateInfo psoCreateInfo;
        psoCreateInfo.PSODesc.Name = entity.getComponent<Tag>().name.c_str();
        psoCreateInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;
        psoCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        auto &desc = Engine::getSwapChain()->GetDesc();
        psoCreateInfo.GraphicsPipeline.RTVFormats[0] = desc.ColorBufferFormat;
        psoCreateInfo.GraphicsPipeline.DSVFormat = desc.DepthBufferFormat;
        psoCreateInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        psoCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_BACK;
        psoCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;

        Diligent::ShaderCreateInfo shaderCI;
        shaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
        shaderCI.UseCombinedTextureSamplers = true;

        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> shaderSourceFactory;
        Engine::getEngineFactory()->CreateDefaultShaderSourceStreamFactory(nullptr, &shaderSourceFactory);
        shaderCI.pShaderSourceStreamFactory = shaderSourceFactory;

        Diligent::RefCntAutoPtr<Diligent::IShader> pVS;
        shaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
        shaderCI.EntryPoint = "main";
        shaderCI.Desc.Name = vs.c_str();
        shaderCI.FilePath = ("shaders/" + vs + ".vsh").c_str();
        Engine::getDevice()->CreateShader(shaderCI, &pVS);
        if (node["UseModelViewProj"].as<bool>()) {
            Diligent::BufferDesc cbDesc;
            cbDesc.Name = "Model View Projection Matrix";
            cbDesc.Size = sizeof(float) * 16;
            cbDesc.Usage = Diligent::USAGE_DYNAMIC;
            cbDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
            cbDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
            Engine::getDevice()->CreateBuffer(cbDesc, nullptr, &entity.addComponent<ModelViewProjMatrix>().vsConstants);
        }

        Diligent::RefCntAutoPtr<Diligent::IShader> pPS;
        shaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
        shaderCI.EntryPoint = "main";
        shaderCI.Desc.Name = fs.c_str();
        shaderCI.FilePath = ("shader/" + vs + ".psh").c_str();
        Engine::getDevice()->CreateShader(shaderCI, &pPS);

        int size = node["LayoutElements"].size();
        std::vector<Diligent::LayoutElement> layoutElements(size / 2);
        for (int i = 0; i < size; i += 2) {
            Diligent::VALUE_TYPE vt;
            std::string name = node["LayoutElements"][i].as<std::string>();
            if (name == "float32") {
                vt = Diligent::VT_FLOAT32;
            } else if (name == "float16") {
                vt = Diligent::VT_FLOAT16;
            } else if (name == "int8") {
                vt = Diligent::VT_INT8;
            } else if (name == "int16") {
                vt = Diligent::VT_INT16;
            } else if (name == "int32") {
                vt = Diligent::VT_INT32;
            } else if (name == "uint8") {
                vt = Diligent::VT_UINT8;
            } else if (name == "uint16") {
                vt = Diligent::VT_UINT16;
            } else if (name == "uint32") {
                vt = Diligent::VT_UINT32;
            }

            layoutElements[i / 2] = {(uint32_t) i / 2, 0, node["LayoutElements"][i + 1].as<uint32_t>(), vt, false};
        }

        psoCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = layoutElements.data();
        psoCreateInfo.GraphicsPipeline.InputLayout.NumElements = layoutElements.size();
        psoCreateInfo.pVS = pVS;
        psoCreateInfo.pPS = pPS;
        psoCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
        Engine::getDevice()->CreateGraphicsPipelineState(psoCreateInfo, &prgm.shaderPointer);
        if (entity.hasComponent<ModelViewProjMatrix>()) {
            prgm.shaderPointer->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "Constants")->Set(
                    entity.getComponent<ModelViewProjMatrix>().vsConstants);
        }
        prgm.shaderPointer->CreateShaderResourceBinding(&prgm.shaderBinding, true);

        auto &transform = entity.addComponent<Transform>();
        transform.position = node["Position"].as<glm::vec3>();
        transform.rotation = glm::quat(node["Rotation"].as<glm::vec3>());

//        prgm.programID = loadProgram(vs.c_str(), fs.c_str());

//        std::getline(open, line);
//        line = line.substr(2, line.length()-4);
//        std::stringstream stringstream;
//        stringstream << line;
//        std::getline(stringstream, line, ',');
//        float x = std::stof(line);
//
//        std::getline(stringstream, line, ',');
//        float y = std::stof(line);
//
//        std::getline(stringstream, line);
//        float z = std::stof(line);
//
//        entity.addComponent<Transform>().position = {x, y, z};
    }

}