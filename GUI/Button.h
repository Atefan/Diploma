#pragma once


#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <bitset>


#include "Obj.h"
#include "ColorUtils.h"
#include "SDLSerialVisualizer.h"
#include "SerialManager.h"


class SDLSerialVisualizer;

class Button : public Obj {
public:
    Button(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id);
    virtual ~Button() {}

    virtual void onClick() = 0;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(SDL_Event* event) override;

    void setColor(SDL_Color newColor);
    void setAvailable(bool isAvailable);
    char* readData(int* dataSize);

protected:
    int id;
    int x, y;
    int width, height;
    SDL_Color color;
    bool available;
    std::string text;
    TTF_Font* font;
};

class Button8Bit : public Button {
public:
    Button8Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id, SDLSerialVisualizer* visualizer);
    void onClick() override;

private:
    SDLSerialVisualizer* myVisualizer;
};

class Button16Bit : public Button {
public:
    Button16Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id);
    void onClick() override;
};

class Button32Bit : public Button {
public:
    Button32Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id);
    void onClick() override;
};