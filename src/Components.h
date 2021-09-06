#ifndef VULKAN_COMPONENTS_H
#define VULKAN_COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <GLFW/glfw3.h>

namespace SGE {
    struct Transform {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        Transform() = default;

        Transform(const Transform &transform) = default;

        glm::mat4 getTransform() const {
            glm::mat4 rot = glm::toMat4(rotation);
            return glm::translate(glm::mat4(1.0f), position) * rot * glm::scale(glm::mat4(1.0f), scale);
        }
    };

    struct Physics {
        glm::vec3 velocity;

        Physics() = default;

        Physics(const Physics &physics) = default;
    };

    struct Tag {
        std::string name;
    };

    struct WindowPtr {
        GLFWwindow *window;
        bool sizeChange = false;
    };

    struct UIComponent {
        float xScale = 1.0, yScale = 1.0;
        int xTop = 0, yTop = 0;
        int xBottom = 0, yBottom = 0;
        //xTop, yTop, xBottom, yBottom, xScale, yScale, width, height
        std::function<void(int&, int&, int&, int&, float, float, int, int)> scalingFunction;
    };
}

#endif //VULKAN_COMPONENTS_H
