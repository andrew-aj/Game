#include "Engine.h"

int main(){
    SGE::Engine gameEngine("Testing", 1280, 720);
    if(!gameEngine.createWindow(GLFW_NO_API) || !gameEngine.initEngine(Diligent::RENDER_DEVICE_TYPE_VULKAN))
        return -1;

    gameEngine.update();

    return 0;
}