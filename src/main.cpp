#include "Engine.h"
#include <typeinfo>
#include <typeindex>

int main(){
    SGE::Engine gameEngine("Testing", 1280, 720);
    if(!gameEngine.createWindow(GLFW_NO_API) || !gameEngine.initEngine(Diligent::RENDER_DEVICE_TYPE_VULKAN))
        return -1;

    gameEngine.update();

    std::type_index a = std::type_index(typeid(int));
    a.name();

    return 0;
}