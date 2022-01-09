#include "ModelLoader.h"

namespace SGE {
    entt::registry *ModelLoader::m_registry;

    std::unique_ptr<ModelLoader> ModelLoader::modelLoader;

    ModelLoader::ModelLoader(entt::registry *registry) {
        m_registry = registry;
        m_deviceClass = DeviceClass::getInstance();
    }

    bool ModelLoader::loadShader(const std::string &loc) {
        YAML::Node original;
        try {
            original = YAML::LoadFile("data/models" + loc);
        } catch (YAML::ParserException &e) {
            boxer::show(e.what(), "Error loading shader");
            return false;
        }

        YAML::Node node = original["ShaderProgram"];
        uint32_t id = node["PregenID"].as<uint32_t>();

        if (meshStorage.find(id) == meshStorage.end()) {
            boxer::show(("You must call loadMesh first for " + loc).c_str(), "Shader Program Error");
            return false;
        }

        std::string vs = node["VertexShader"].as<std::string>();

        std::string fs = node["FragmentShader"].as<std::string>();

        Program prgm {programStorage[id].first, programStorage[id].second};

        Diligent::GraphicsPipelineStateCreateInfo psoCreateInfo;
        psoCreateInfo.PSODesc.Name = loc.c_str();
        psoCreateInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;
        psoCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        auto &desc = m_deviceClass->m_pSwapChain->GetDesc();
        psoCreateInfo.GraphicsPipeline.RTVFormats[0] = desc.ColorBufferFormat;
        psoCreateInfo.GraphicsPipeline.DSVFormat = desc.DepthBufferFormat;
        psoCreateInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        psoCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_BACK;
        psoCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;

        Diligent::ShaderCreateInfo shaderCI;
        shaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
        shaderCI.UseCombinedTextureSamplers = true;

        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> shaderSourceFactory;
        m_deviceClass->m_pDevice->GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(nullptr, &shaderSourceFactory);
        shaderCI.pShaderSourceStreamFactory = shaderSourceFactory;

        Diligent::RefCntAutoPtr<Diligent::IShader> pVS;
        shaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
        shaderCI.EntryPoint = "main";
        shaderCI.Desc.Name = vs.c_str();
        shaderCI.FilePath = ("shaders/" + vs + ".vsh").c_str();
        m_deviceClass->m_pDevice->CreateShader(shaderCI, &pVS);
        if (node["UseModelViewProj"].as<bool>()) {
            Diligent::BufferDesc cbDesc;
            cbDesc.Name = "Model View Projection Matrix";
            cbDesc.Size = sizeof(float) * 16;
            cbDesc.Usage = Diligent::USAGE_DYNAMIC;
            cbDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
            cbDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
            m_deviceClass->m_pDevice->CreateBuffer(cbDesc, nullptr, &projMatrixStorage[id]);
        }

        Diligent::RefCntAutoPtr<Diligent::IShader> pPS;
        shaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
        shaderCI.EntryPoint = "main";
        shaderCI.Desc.Name = fs.c_str();
        shaderCI.FilePath = ("shader/" + vs + ".psh").c_str();
        m_deviceClass->m_pDevice->CreateShader(shaderCI, &pPS);

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
        m_deviceClass->m_pDevice->CreateGraphicsPipelineState(psoCreateInfo, &prgm.shaderPointer);
        if (projMatrixStorage.find(id) != projMatrixStorage.end()) {
            prgm.shaderPointer->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "Constants")->Set(
                    projMatrixStorage[id]);
        }
        prgm.shaderPointer->CreateShaderResourceBinding(&prgm.shaderBinding, true);
        return true;
    }

    ModelLoader *ModelLoader::createInstance(entt::registry *registry) {
        if (!modelLoader) {
            modelLoader = std::make_unique<ModelLoader>(registry);
        }
        return modelLoader.get();
    }

    ModelLoader *ModelLoader::createInstance() {
        if (!modelLoader && modelLoader->m_registry == nullptr)
            return nullptr;
        else
            return modelLoader.get();
    }

    bool ModelLoader::loadMesh(const std::string &loc) {
        YAML::Node original;
        try {
            original = YAML::LoadFile("data/models" + loc);
        } catch (YAML::ParserException &e) {
            boxer::show(e.what(), "Error loading mesh");
            return false;
        }

        YAML::Node node = original["Mesh"];

        uint32_t id = node["PregenID"].as<uint32_t>();
        auto &mesh = meshStorage[id];

        int verticesCount = node["NumVerts"].as<int>();
        mesh.vertices.reserve(verticesCount);

        int length = node["NumIndices"].as<int>();
        mesh.indices.reserve(length);

//        mesh.vertices = node["Vertices"].as<std::vector<Vertex>>();
        std::vector<float> temp = node["Vertices"].as<std::vector<float>>();
        mesh.vertices;

        mesh.indices = node["Indices"].as<std::vector<unsigned short>>();
        return true;
    }

    bool ModelLoader::loadVertexBuffer(const std::string &loc) {
        YAML::Node original;
        try {
            original = YAML::LoadFile("data/models" + loc);
        } catch (YAML::ParserException &e) {
            boxer::show(e.what(), "Error loading vertex buffer");
            return false;
        }

        YAML::Node node = original["VertexBuffer"];
        uint32_t id = node["PregenID"].as<uint32_t>();

        if (meshStorage.find(id) == meshStorage.end()) {
            boxer::show(("You must call loadMesh first for " + loc).c_str(), "Vertex Buffer Error");
            return false;
        }

        auto &ref = vbStorage[id];
        auto &mesh = meshStorage[id];
        Diligent::BufferDesc VertBuffDesc;
        VertBuffDesc.Name = loc.c_str();
        VertBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
        VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
        VertBuffDesc.Size = mesh.vertices.size() * sizeof(Vertex);
        Diligent::BufferData VBData;
        VBData.pData = mesh.vertices.data();
        VBData.DataSize = mesh.vertices.size() * sizeof(Vertex);
        m_deviceClass->m_pDevice->CreateBuffer(VertBuffDesc, &VBData, &ref);

        return true;
    }

    bool ModelLoader::loadIndexBuffer(const std::string &loc) {
        YAML::Node original;
        try {
            original = YAML::LoadFile("data/models" + loc);
        } catch (YAML::ParserException &e) {
            boxer::show(e.what(), "Error loading index buffer");
            return false;
        }

        YAML::Node node = original["IndexBuffer"];
        uint32_t id = node["PregenID"].as<uint32_t>();

        if (meshStorage.find(id) == meshStorage.end()) {
            boxer::show(("You must call loadMesh first for " + loc).c_str(), "Index Buffer Error");
            return false;
        }

        auto &ref = ibStorage[id];
        auto &mesh = meshStorage[id];
        Diligent::BufferDesc IndBuffDesc;
        IndBuffDesc.Name = loc.c_str();
        IndBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
        IndBuffDesc.BindFlags = Diligent::BIND_INDEX_BUFFER;
        IndBuffDesc.Size = mesh.indices.size() * sizeof(short);
        Diligent::BufferData IBData;
        IBData.pData = mesh.indices.data();
        IBData.DataSize = mesh.indices.size() * sizeof(short);
        m_deviceClass->m_pDevice->CreateBuffer(IndBuffDesc, &IBData, &ref);

        return true;
    }

    MeshComponent &ModelLoader::getMesh(uint32_t id) {
        return meshStorage[id];
    }

    VertexBuffer ModelLoader::getVertexBuffer(uint32_t id) {
        return {vbStorage[id]};
    }

    IndexBuffer ModelLoader::getIndexBuffer(uint32_t id) {
        return {ibStorage[id]};
    }

    Program ModelLoader::getShaderProgram(uint32_t id) {
        return {programStorage[id].first, programStorage[id].second};
    }

    ModelViewProjMatrix ModelLoader::getProjectionMatrix(uint32_t id) {
        return {projMatrixStorage[id]};
    }

}