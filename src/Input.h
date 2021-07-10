#ifndef VULKAN_INPUT_H
#define VULKAN_INPUT_H

#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include <functional>

namespace SGE {

    class Input {
    public:
        Input(const std::vector<int>& keysToMonitor, const std::vector<int>& mouseButtonsToMonitor);

        ~Input();

        bool getIsKeyDown(int key);

        std::pair<double, double> getMousePos();

        std::pair<double, double> getMouseOffset();

        std::pair<double, double> getScroll();

        bool getIsMouseButtonDown(int key);

        static void setUpInputs(GLFWwindow *window);

        static void setUpMouseInputs(GLFWwindow *window);

        static void setUpMouseButton(GLFWwindow *window);

        static void setUpScroll(GLFWwindow *window);
    private:
        static void callBack(GLFWwindow *window, int key, int scancode, int action, int mods);

        static void mouseCallBack(GLFWwindow *window, double xpos, double ypos);

        static void mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods);

        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

        void setIsKeyDown(int key, bool isDown);

        void setMouseButtonIsDown(int key, bool isDown);

        static std::vector<Input *> _instances;
        std::map<int, bool> _keys;
        std::map<int, bool> _mouseButtons;
        bool _isEnabled;

        bool firstMouse = true;
        double lastX = 0, lastY = 0;
        double xOffset = 0, yOffset = 0;
        double xScroll = 0, yScroll = 0;
    };

    std::vector<Input *> Input::_instances;

    Input::Input(const std::vector<int>& keysToMonitor, const std::vector<int>& mouseButtonsToMonitor) : _isEnabled(true) {
        for (int key : keysToMonitor) {
            _keys[key] = false;
        }
        for (int key : mouseButtonsToMonitor) {
            _mouseButtons[key] = false;
        }
        _instances.push_back(this);
    }

    Input::~Input() {
        _instances.erase(std::remove(_instances.begin(), _instances.end(), this), Input::_instances.end());
    }

    bool Input::getIsKeyDown(int key) {
        bool result = false;
        if (_isEnabled) {
            auto it = _keys.find(key);
            if (it != _keys.end()) {
                result = _keys[key];
            }
        }
        return result;
    }

    std::pair<double, double> Input::getMousePos() {
        return {lastX, lastY};
    }

    std::pair<double, double> Input::getMouseOffset() {
        return {xOffset, yOffset};
    }

    std::pair<double, double> Input::getScroll() {
        return {xScroll, yScroll};
    }

    bool Input::getIsMouseButtonDown(int key) {
        bool result = false;
        if (_isEnabled) {
            auto it = _mouseButtons.find(key);
            if (it != _mouseButtons.end()) {
                result = _mouseButtons[key];
            }
        }
        return result;
    }

    void Input::setUpInputs(GLFWwindow *window) {
        glfwSetKeyCallback(window, callBack);
    }

    void Input::setUpMouseInputs(GLFWwindow *window) {
        glfwSetCursorPosCallback(window, mouseCallBack);
    }

    void Input::setUpMouseButton(GLFWwindow *window) {
        glfwSetMouseButtonCallback(window, mouseButtonCallBack);
    }

    void Input::setUpScroll(GLFWwindow *window) {
        glfwSetScrollCallback(window, scroll_callback);
    }

    void Input::callBack(GLFWwindow *window, int key, int scancode, int action, int mods) {
        for (auto input : _instances) {
            input->setIsKeyDown(key, action != GLFW_RELEASE);
        }
    }

    void Input::mouseCallBack(GLFWwindow *window, double xpos, double ypos) {
        for (auto input : _instances) {
            input->firstMouse = false;
            input->xOffset = xpos - input->lastX;
            input->yOffset = ypos - input->lastY;
            input->lastX = xpos;
            input->lastY = ypos;
        }
    }

    void Input::mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods) {
        for (auto input : _instances) {
            input->setMouseButtonIsDown(button, action != GLFW_RELEASE);
        }
    }

    void Input::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        for (auto input : _instances) {
            input->xScroll = xoffset;
            input->yScroll = yoffset;
        }
    }

    void Input::setIsKeyDown(int key, bool isDown) {
        auto it = _keys.find(key);
        if (it != _keys.end()) {
            _keys[key] = isDown;
        }
    }

    void Input::setMouseButtonIsDown(int key, bool isDown) {
        auto it = _mouseButtons.find(key);
        if (it != _mouseButtons.end()) {
            _mouseButtons[key] = isDown;
        }
    }

}


#endif //VULKAN_INPUT_H
