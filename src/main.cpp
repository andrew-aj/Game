#include "Engine.h"

int main(){
    SGE::Engine gameEngine("Strategy Game", 1280, 720);
    if(!gameEngine.createWindow() || !gameEngine.initEngine())
        return -1;

    gameEngine.update();

    return 0;
}