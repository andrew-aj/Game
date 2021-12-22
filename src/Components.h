#ifndef VULKAN_COMPONENTS_H
#define VULKAN_COMPONENTS_H

#include <string>
#include <iostream>
#include "Includes.h"

namespace SGE {
    struct Transform {
        glm::vec3 position = {0, 0, 0};
        glm::quat rotation = glm::quat({0.f, 0.f, 0.f});
        glm::vec3 scale = {1, 1, 1};

        Transform() = default;

        Transform(const Transform &transform) = default;

        glm::mat4 getTransform() const {
            glm::mat4 rot = glm::toMat4(rotation);
            return glm::translate(glm::mat4(1.0f), position) * rot * glm::scale(glm::mat4(1.0f), scale);
        }
    };

    struct Physics {
        glm::vec3 velocity;
        glm::vec3 acceleration;

        Physics() = default;

        Physics(const Physics &physics) = default;
    };

    struct Tag {
        std::string name;
    };

    struct WindowPtr {
        GLFWwindow *window;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_Device;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_ImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_SwapChain;
        bool sizeChange = false;
        bool running = true;
    };

    struct UIComponent {
        float xScale = 1.0, yScale = 1.0;
        int xTop = 0, yTop = 0;
        int xBottom = 0, yBottom = 0;
        //xTop, yTop, xBottom, yBottom, xScale, yScale, width, height
        std::function<void(int &, int &, int &, int &, float, float, int, int)> scalingFunction;
    };

    struct ModelComponent {
        int *mesh;
    };

    struct Vertex {
        glm::vec3 pos;
        glm::vec4 texCoord;

        bool operator==(const Vertex &other) const {
            return pos == other.pos && texCoord == other.texCoord;
        }
    };

    struct MeshComponent {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
    };

    struct Program {
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> shaderPointer;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> shaderBinding;
    };

    struct ModelViewProjMatrix{
        Diligent::RefCntAutoPtr<Diligent::IBuffer> vsConstants;
    };

    struct VertexBuffer {
        Diligent::RefCntAutoPtr<Diligent::IBuffer> vertexBuffer;
    };

    struct IndexBuffer {
        Diligent::RefCntAutoPtr<Diligent::IBuffer> indexBuffer;
    };

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 viewProj;
    };

    struct Time {
        float dt = 0;
        float lastFrame = 0;
    };

    struct CameraComponent {
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        bool smooth;
        float zoom;
    };

    struct PrimaryController {
        bool correct = true;
    };

    struct AttachedTo {
        entt::entity target = entt::null;
    };

    struct MovementDisabled {

    };
}

#endif //VULKAN_COMPONENTS_H
