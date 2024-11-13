#pragma once

#include <vector>
#include "Obj.h"
#include <SDL.h>
#include <SDL_ttf.h>

#include "serialib.h"

#include "SerialManager.h"
#include "NumberDisplay.h"

class SDLSerialVisualizer {
public:
    SDLSerialVisualizer();
    ~SDLSerialVisualizer();

    bool setup();
    void run();
    std::vector<Obj*> getObjects();

private:
    void createButtons(TTF_Font* font);
    char* readData(int* dataSize);

    std::vector<Obj*> objects;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool quit;
    const char* port;
};
