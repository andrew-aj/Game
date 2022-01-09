#include "Input.h"

namespace SGE {
    std::vector<Input *> Input::_instances;

    double Input::lastX = 0, Input::lastY = 0, Input::xOffset = 0, Input::yOffset = 0, Input::xScroll = 0, Input::yScroll = 0;

    Input::Input(const std::vector<int> &keysToMonitor, const std::vector<int> &mouseButtonsToMonitor) : _isEnabled(
            true) {
        for (int key : keysToMonitor) {
            _keys[key] = false;
        }
        for (int key : mouseButtonsToMonitor) {
            _mouseButtons[key] = false;
        }
        _instances.push_back(this);
    }

    Input::Input(const std::vector<int> &keysToMonitor) : Input(keysToMonitor, {}) {}

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
        std::pair<double, double> temp = {xScroll, yScroll};
        xScroll = 0;
        yScroll = 0;
        return temp;
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
            if (input->firstMouse) {
                input->lastX = xpos;
                input->lastY = ypos;
                input->firstMouse = false;
            }
            if (xpos != input->lastX) {
                input->xOffset = xpos - input->lastX;
                input->xposUpdate = true;
            }
            if (ypos != input->lastY) {
                input->yOffset = ypos - input->lastY;
                input->yposUpdate = true;
            }
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
        Input::xScroll = xoffset;
        Input::yScroll = yoffset;
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

    bool Input::keyIsDown() {
        return !_keys.empty();
    }
}