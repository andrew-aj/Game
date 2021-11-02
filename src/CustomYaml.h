#ifndef VULKAN_CUSTOMYAML_H
#define VULKAN_CUSTOMYAML_H

#include <glm/glm.hpp>
#include "Components.h"

namespace YAML {
    template<>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3 &rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node &node, glm::vec3 &rhs) {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::quat> {
        static Node encode(const glm::quat &rhs) {
            Node node;
            node.push_back(rhs.w);
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node &node, glm::quat &rhs) {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.w = node[0].as<float>();
            rhs.x = node[1].as<float>();
            rhs.y = node[2].as<float>();
            rhs.z = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<SGE::CameraComponent> {
        static Node encode(const SGE::CameraComponent &rhs) {
            Node node;
            node.push_back(rhs.position);
            node.push_back(rhs.front);
            node.push_back(rhs.up);
            node.push_back(rhs.right);
            node.push_back(rhs.smooth);
            node.push_back(rhs.zoom);
            return node;
        }

        static bool decode(const Node &node, SGE::CameraComponent &rhs) {
            if (!node.IsMap() || node.size() != 6) {
                return false;
            }

            rhs.position = node["position"].as<glm::vec3>();
            rhs.front = node["front"].as<glm::vec3>();
            rhs.up = node["up"].as<glm::vec3>();
            rhs.right = node["right"].as<glm::vec3>();
            rhs.smooth = node["smooth"].as<bool>();
            rhs.zoom = node["zoom"].as<float>();
            return true;
        }
    };

    template<>
    struct convert<SGE::Transform> {
        static Node encode(const SGE::Transform &rhs) {
            Node node;
            node.push_back(rhs.position);
            node.push_back(rhs.rotation);
            node.push_back(rhs.scale);
            return node;
        }

        static bool decode(const Node &node, SGE::Transform &rhs) {
            if (!node.IsMap() || node.size() != 3) {
                return false;
            }

            rhs.position = node["position"].as<glm::vec3>();
            rhs.rotation = node["rotation"].as<glm::quat>();
            rhs.scale = node["scale"].as<glm::vec3>();
            return true;
        }
    };

    template<>
    struct convert<SGE::Physics> {
        static Node encode(const SGE::Physics &rhs) {
            Node node;
            node.push_back(rhs.velocity);
            node.push_back(rhs.acceleration);
            return node;
        }

        static bool decode(const Node &node, SGE::Physics &rhs) {
            if (!node.IsMap() || node.size() != 2) {
                return false;
            }

            rhs.velocity = node["velocity"].as<glm::vec3>();
            rhs.acceleration = node["acceleration"].as<glm::vec3>();
            return true;
        }
    };

    template<>
    struct convert<SGE::AttachedTo> {
        static Node encode(const SGE::AttachedTo &rhs) {
            Node node;
            node.push_back(rhs.target);
            return node;
        }

        static bool decode(const Node &node, SGE::AttachedTo &rhs) {
            if (!node.IsScalar()) {
                return false;
            }

            rhs.target = node.as<entt::entity>();
            return true;
        }
    };

    template<>
    struct convert<entt::entity> {
        static Node encode(const entt::entity &rhs) {
            Node node;
            node.push_back((uint32_t) rhs);
            return node;
        }

        static bool decode(const Node &node, entt::entity &rhs) {
            if (!node.IsScalar()) {
                return false;
            }

            std::string temp = node.Tag();
            if (node.as<std::string>() == "none"){
                rhs = entt::null;
                return true;
            }
            rhs = (entt::entity) node.as<uint32_t>();
            return true;
        }
    };

    template<>
    struct convert<SGE::PrimaryController> {
        static Node encode(const SGE::PrimaryController &rhs) {
            Node node;
            node.push_back(true);
            return node;
        }

        static bool decode(const Node &node, SGE::PrimaryController &rhs) {
            if (!node.IsScalar()) {
                return false;
            }

            rhs.correct = node.as<bool>();
            return true;
        }
    };

    template<>
    struct convert<SGE::Tag> {
        static Node encode(const SGE::Tag &rhs) {
            Node node;
            node.push_back(rhs.name);
            return node;
        }

        static bool decode(const Node &node, SGE::Tag &rhs) {
            if (!node.IsScalar()) {
                return false;
            }

            rhs.name = node.as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<SGE::Time> {
        static Node encode(const SGE::Time &rhs) {
            Node node;
            node.push_back(rhs.dt);
            node.push_back(rhs.lastFrame);
            return node;
        }

        static bool decode(const Node &node, SGE::Time &rhs) {
            if (!node.IsMap() || node.size() != 2) {
                return false;
            }

            rhs.dt = node["dt"].as<float>();
            rhs.lastFrame = node["lastFrame"].as<float>();
            return true;
        }
    };

    template<>
    struct convert<SGE::WindowPtr> {
        static Node encode(const SGE::WindowPtr &rhs) {
            Node node;
            node.push_back(rhs.sizeChange);
            node.push_back(rhs.running);
            return node;
        }

        static bool decode(const Node &node, SGE::WindowPtr &rhs) {
            if (!node.IsMap() || node.size() != 2) {
                return false;
            }

            rhs.sizeChange = node["sizeChange"].as<bool>();
            rhs.running = node["running"].as<bool>();
            return true;
        }
    };

    template<>
    struct convert<SGE::UIComponent> {
        static Node encode(const SGE::UIComponent &rhs) {
            Node node;
            node.push_back(rhs.xScale);
            node.push_back(rhs.yScale);
            node.push_back(rhs.xTop);
            node.push_back(rhs.yTop);
            node.push_back(rhs.xBottom);
            node.push_back(rhs.yBottom);
            return node;
        }

        static bool decode(const Node &node, SGE::UIComponent &rhs) {
            if (!node.IsMap() || node.size() != 6) {
                return false;
            }

            rhs.xScale = node["xScale"].as<float>();
            rhs.yScale = node["yScale"].as<float>();
            rhs.xTop = node["xTop"].as<int>();
            rhs.yTop = node["yTop"].as<int>();
            rhs.xBottom = node["xBottom"].as<int>();
            rhs.yBottom = node["yBottom"].as<int>();
            return true;
        }
    };
}

#endif //VULKAN_CUSTOMYAML_H
