#pragma once

#include <vector>
#include "Obj.h"
#include <SDL.h>
#include <SDL_ttf.h>

#include "NumberDisplay.h"
#include "Constants.h"

class SDLVisualizer {
public:
    SDLVisualizer();
    ~SDLVisualizer();

    bool setup();
    void run();
    std::vector<Obj*> getObjects();

private:
    void createButtons(TTF_Font* font);

    std::vector<Obj*> objects;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool quit;
    const char* file_name;
};
