#include <iostream>
#include <bitset>

#include "SDLSerialVisualizer.h"
#include "Button.h"
#include "Constants.h"



SDLSerialVisualizer::SDLSerialVisualizer()
    : window(nullptr), renderer(nullptr), quit(false), port(COM) {}

SDLSerialVisualizer::~SDLSerialVisualizer() {
    for (auto obj : objects) {
        delete obj;
    }
    objects.clear();
    TTF_Quit();
    serial.closeDevice();
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool SDLSerialVisualizer::setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow("Visualizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << "\n";
        return false;
    }

    TTF_Font* font = TTF_OpenFont("./fonts/CalligraphyFLF.ttf", 24);
    if (!font) {
        std::cout << "Failed to load font! TTF_Error: " << TTF_GetError() << "\n";
        return false;
    }

    createButtons(font);

    if (serial.openDevice(port, 115200) != 1) {
        std::cout << "Error opening port\n";
        return false;
    }

    return true;
}

void SDLSerialVisualizer::run() {
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            for (auto obj : objects)
                obj->handleEvent(&e);
        }

        for (auto obj : objects) {
            obj->process();
        }

        for (auto obj : objects)
            obj->render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(5);
    }
}

std::vector<Obj*> SDLSerialVisualizer::getObjects() {
    return objects;
}

void SDLSerialVisualizer::createButtons(TTF_Font* font) {

    int buttonStartX = (SCREEN_WIDTH / 2 - (BUTTON_SPACING * 2 + BUTTON_WIDTH * 2.5));


    Button* myButton = nullptr;
    SDL_Color buttonColor = getColor(Color::GRAY4);

    myButton = new Button8Bit(buttonStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, buttonColor, true, "8 Bit", font, this);
    objects.push_back(myButton);

    buttonColor = getColor(Color::GRAY4);
    myButton = new Button16Bit(buttonStartX + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, buttonColor, true, "16 Bit", font, this);
    objects.push_back(myButton);

    buttonColor = getColor(Color::GRAY4);
    myButton = new Button32Bit(buttonStartX + 2 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, buttonColor, true, "32 Bit", font, this);
    objects.push_back(myButton);

    buttonColor = getColor(Color::GRAY3);
    myButton = new ButtonStreamBit(buttonStartX + 3 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, buttonColor, false, "Stream", font, this);
    objects.push_back(myButton);

    buttonColor = getColor(Color::GRAY3);
    myButton = new ButtonFile(buttonStartX + 4 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, buttonColor, false, "File", font, this);
    objects.push_back(myButton);

    buttonColor = getColor(Color::WHITE);
    NumberDisplay* myNumberDisplay = new NumberDisplay(
        (SCREEN_WIDTH - DISPLAY_WIDTH) / 2,
        DISPLAY_Y,                           
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        Color::WHITE,
        font
    );
    objects.push_back(myNumberDisplay);
}
