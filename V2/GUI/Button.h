#pragma once


#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <bitset>
#include <fstream>

#include "Obj.h"
#include "ColorUtils.h"
#include "SDLSerialVisualizer.h"
#include "fd.h"


class SDLSerialVisualizer;

class Button : public Obj {
public:
    Button(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font);
    virtual ~Button() {}

    virtual void onClick() = 0;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(SDL_Event* event) override;

    void setColor(SDL_Color newColor);
    void setAvailable(bool isAvailable);
    int readData(int dataSize);

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
    Button8Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLSerialVisualizer* visualizer);
    void onClick() override;
    void process() override;

private:
    SDLSerialVisualizer* myVisualizer;
};

class Button16Bit : public Button {
public:
    Button16Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLSerialVisualizer* visualizer);
    void onClick() override;
    void process() override;

private:
    SDLSerialVisualizer* myVisualizer;
};

class Button32Bit : public Button {
public:
    Button32Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLSerialVisualizer* visualizer);
    void onClick() override;
    void process() override;

private:
    SDLSerialVisualizer* myVisualizer;
};

class ButtonStreamBit : public Button {
public:
    ButtonStreamBit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLSerialVisualizer* visualizer);
    void onClick() override;
    void process() override;
    ~ButtonStreamBit() override;

private:
    uint32_t* buffer;
    std::ofstream binary_file;
    int index, currentSize, bufferSize, bufferIndex;
    SDLSerialVisualizer* myVisualizer;
};


class ButtonFile : public Button {
public:
    ButtonFile(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLSerialVisualizer* visualizer);
    void onClick() override;
    void process() override;
    ~ButtonFile() override;

private:
    std::ofstream file;
    SDLSerialVisualizer* myVisualizer;
};