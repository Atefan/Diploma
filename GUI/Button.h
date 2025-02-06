#pragma once


#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <bitset>
#include <fstream>

#include "Obj.h"
#include "ColorUtils.h"
#include "SDLVisualizer.h"
#include "Constants.h"
#include "fd.h"


class SDLVisualizer;

class Button : public Obj {
public:
    Button(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font);
    virtual ~Button() {}

    virtual void onClick() = 0;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(SDL_Event* event) override;

    void setColor(SDL_Color newColor);
    void setAvailable(bool isAvailable);
    int readData(void* buffer, int bufferSize);

protected:
    int x, y;
    int width, height;
    SDL_Color color;
    bool active;
    std::string text;
    TTF_Font* font;
};

class Button8Bit : public Button {
public:
    Button8Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLVisualizer* visualizer);
    void onClick() override;
    void process() override;

private:
    SDLVisualizer* myVisualizer;
};

class Button16Bit : public Button {
public:
    Button16Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLVisualizer* visualizer);
    void onClick() override;
    void process() override;

private:
    SDLVisualizer* myVisualizer;
};

class Button32Bit : public Button {
public:
    Button32Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLVisualizer* visualizer);
    void onClick() override;
    void process() override;

private:
    SDLVisualizer* myVisualizer;
};

class ButtonBitMap : public Button {
public:
    ButtonBitMap(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLVisualizer* visualizer);
    void onClick() override;
    void process() override;
    ~ButtonBitMap() override;

private:
    uint8_t* buffer;
    SDLVisualizer* myVisualizer;
};


class ButtonFile : public Button {
public:
    ButtonFile(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLVisualizer* visualizer);
    void onClick() override;
    void process() override;
    ~ButtonFile() override;

private:
    std::ofstream file;
    SDLVisualizer* myVisualizer;
};