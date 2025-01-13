#pragma once
#include <SDL.h>

class Obj {
public:
    virtual ~Obj() {}
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void handleEvent(SDL_Event* event) = 0;
    virtual void process() = 0;
};