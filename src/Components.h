#ifndef VULKAN_COMPONENTS_H
#define VULKAN_COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <GLFW/glfw3.h>
#include <Entt/entt.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace SGE {
    struct Transform {
        glm::vec3 position = {0, 0, 0};
        glm::quat rotation = glm::quat({0.f, 0.f,0.f});
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
        Mesh *mesh;
    };

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }

        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    };

    struct MeshComponent {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
    };

    struct Program {
        bgfx::ProgramHandle programID;
    };

    struct VertexBuffer {
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
    };

    struct IndexBuffer {
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
    };

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
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

    struct AttachedTo{
        entt::entity target = entt::null;
    };

    struct MovementDisabled{

    };
}

#endif //VULKAN_COMPONENTS_H
