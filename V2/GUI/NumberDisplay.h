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
    void updateNumber(uint32_t newNumber, int newBits);
    void render(SDL_Renderer* renderer) override;
    void handleEvent(SDL_Event* event) override;
    void process() override;

private:
    void num_to_string(std::string& binaryStr, std::string& numberStr, uint32_t number);

    int x, y;
    int width, height;
    SDL_Color color;
    TTF_Font* font;
    uint32_t number;
    int bits;
};
