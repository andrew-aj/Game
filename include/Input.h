#ifndef VULKAN_INPUT_H
#define VULKAN_INPUT_H

#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include "Includes.h"

namespace SGE {

    class Input {
    public:
        bool xposUpdate = false;
        bool yposUpdate = false;

        Input(const std::vector<int> &keysToMonitor, const std::vector<int> &mouseButtonsToMonitor);

        Input(const std::vector<int> &keysToMonitor);

        ~Input();

        bool getIsKeyDown(int key);

        static std::pair<double, double> getMousePos();

        static std::pair<double, double> getMouseOffset();

        static std::pair<double, double> getScroll();

        bool getIsMouseButtonDown(int key);

        bool keyIsDown();

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
        static double lastX, lastY;
        static double xOffset, yOffset;
        static double xScroll, yScroll;
    };

}


#endif //VULKAN_INPUT_H
