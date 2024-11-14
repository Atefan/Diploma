#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>

#include "ColorUtils.h"
#include "Obj.h"


class NumberDisplay : public Obj {
public:
    NumberDisplay(int x, int y, int width, int height, Color color, TTF_Font* font);
    void updateNumber(int newNumber, int newBits);
    void render(SDL_Renderer* renderer) override;
    void handleEvent(SDL_Event* event) override;

private:
    void num_to_string(std::string& binaryStr, std::string& numberStr, int number);

    int x, y;
    int width, height;
    SDL_Color color;
    TTF_Font* font;
    int number;
    int bits;
};
