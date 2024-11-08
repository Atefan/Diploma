#include <iostream>
#include <bitset>
#include <SDL.h>
#include "serialib.h"
#include <SDL_ttf.h>
#include <vector>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define BORDER_SIZE 700

#define BUTTON_8 12345
#define BUTTON_16 2345
#define BUTTON_32 3456
#define BUTTON_64 4567
#define BUTTON_STREAM 5678
#define BUTTON_FILE 6789


enum class Color {
    RED,
    GREEN,
    BLUE,
    WHITE,
    BLACK,
    YELLOW,
    CYAN,
    MAGENTA
};

SDL_Color getColor(Color color) {
    switch (color) {
    case Color::RED:      return { 255, 0, 0, 255 };
    case Color::GREEN:    return { 0, 255, 0, 255 };
    case Color::BLUE:     return { 0, 0, 255, 255 };
    case Color::WHITE:    return { 255, 255, 255, 255 };
    case Color::BLACK:    return { 0, 0, 0, 255 };
    case Color::YELLOW:   return { 255, 255, 0, 255 };
    case Color::CYAN:     return { 0, 255, 255, 255 };
    case Color::MAGENTA:  return { 255, 0, 255, 255 };
    default:              return { 0, 0, 0, 255 }; // Default to black if unknown
    }
}

class Matrix {
public:
    Matrix(int width, int height) : width(width), height(height), last_index(0) {}

    void render(SDL_Renderer* renderer, char* data, int size) {
        // Draw white border
        int xOffset = (SCREEN_WIDTH - BORDER_SIZE) / 2;
        int yOffset = (SCREEN_HEIGHT - BORDER_SIZE) / 2;
        SDL_Rect borderRect = { xOffset, yOffset, BORDER_SIZE, BORDER_SIZE };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &borderRect);

        int numCols = BORDER_SIZE;

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < 8; ++j) {
                bool bit = (data[i] >> j) & 1;
                int row = (last_index / numCols);
                int col = (last_index % numCols);

                // Bounds check
                if (row >= BORDER_SIZE || col >= BORDER_SIZE) {
                    break;
                }

                // Set color based on bit value and draw point
                SDL_SetRenderDrawColor(renderer, bit ? 255 : 0, 0, bit ? 0 : 255, 255);
                SDL_RenderDrawPoint(renderer, xOffset + col, yOffset + row);

                ++last_index;
                if (last_index >= BORDER_SIZE * BORDER_SIZE) {
                    break;
                }
            }
            if (last_index >= BORDER_SIZE * BORDER_SIZE) {
                break;
            }
        }

        // Update screen to show drawing
        SDL_RenderPresent(renderer);
    }

    int last_index;

private:
    int width;
    int height;
};

class Obj {

public:
    virtual ~Obj() {}

    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void handleEvent(SDL_Event* event) = 0;
};


class Button :public Obj {
public:
    Button(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id)
        : x(x), y(y), width(width), height(height), color(color), available(available), text(text), font(font), id(id) {}

    void render(SDL_Renderer* renderer) override {
        if (!available) return;

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect buttonRect = { x, y, width, height };
        SDL_RenderFillRect(renderer, &buttonRect);

        SDL_Color textColor = { 255, 255, 255, 255 };  // White
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            int textWidth = textSurface->w;
            int textHeight = textSurface->h;
            SDL_FreeSurface(textSurface);

            SDL_Rect textRect = { x + (width - textWidth) / 2, y + (height - textHeight) / 2, textWidth, textHeight };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

            SDL_DestroyTexture(textTexture);
        }
    }

    void handleEvent(SDL_Event* event) override {
        if (!available) return;

        if (event->type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            if (mouseX >= x && mouseX <= x + width &&
                mouseY >= y && mouseY <= y + height) {
                onClick();
            }
        }
    }

    void setColor(SDL_Color newColor) {
        color = newColor;
    }

    void setAvailable(bool isAvailable) {
        available = isAvailable;
    }

    void onClick() {
        std::cout << "Button " << id <<" clicked!" << std::endl;
    }

private:
    int id;
    int x, y;
    int width, height;
    SDL_Color color;
    bool available;
    std::string text;
    TTF_Font* font;
};

class SDLSerialVisualizer {
public:

    bool StopWhenFiled = true;

    SDLSerialVisualizer(const char* port, int baudRate)
        : port(port), baudRate(baudRate), window(nullptr), renderer(nullptr), quit(false) {}

    ~SDLSerialVisualizer() {
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

    bool setup() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
            return false;
        }

        // Create a full-screen window
        SDL_Window* window = SDL_CreateWindow("Full Screen Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }

        if (window == nullptr) {
            std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
            return false;
        }

        if (TTF_Init() == -1) {
            std::cout << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << "\n";
            return false;
        }
        
        TTF_Font* font = TTF_OpenFont("./fonts/CalligraphyFLF.ttf", 24);  // Specify the correct path to your TTF font
        if (!font) {
            std::cout << "Failed to load font! TTF_Error: " << TTF_GetError() << "\n";
            return false;
        }

        createButtons(font);
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (serial.openDevice(port, baudRate) != 1) {
            std::cout << "Error opening port\n";
            return false;
        }

        return true;
    }

    void run() {
        SDL_Event e;

        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
                for (auto obj : objects) 
                    obj->handleEvent(&e);
            }

            for (auto obj : objects)
                obj->render(renderer);


            SDL_RenderPresent(renderer);
        }
    }

    // Read data from the COM port and convert it to uint8_t
    char* readData(int* dataSize) {
        const int bytesToRead = 64;
        char* data = new char[bytesToRead];
        int bytesRead;
        do {
            bytesRead = serial.readBytes(data, bytesToRead, 1000);

            *dataSize = bytesRead;
            std::cout << "Data read: ";
            for (int i = 0; i < bytesRead; ++i) 
                std::cout << std::bitset<8>(data[i]) << " ";
            
            std::cout << std::endl;
        } while (bytesRead <= 0 || *data < 0);
        
        return data;
    }

private:
    void createButtons(TTF_Font* font)
    {
        Button* myButton = 0;
        SDL_Color buttonColor;

        buttonColor = getColor(Color::RED);
        myButton = new Button(200, 150, 100, 50, buttonColor, true, "8 Bit", font, BUTTON_8);
        objects.push_back(myButton);

        buttonColor = getColor(Color::GREEN);
        myButton = new Button(350, 150, 100, 50, buttonColor, true, "16 Bit", font, BUTTON_16);
        objects.push_back(myButton);

        buttonColor = getColor(Color::BLUE);
        myButton = new Button(600, 150, 100, 50, buttonColor, true, "32 Bit", font, BUTTON_32);
        objects.push_back(myButton);

        buttonColor = getColor(Color::CYAN);
        myButton = new Button(200, 300, 100, 50, buttonColor, true, "64 Bit", font, BUTTON_64);
        objects.push_back(myButton);

        buttonColor = getColor(Color::MAGENTA);
        myButton = new Button(350, 300, 100, 50, buttonColor, true, "Stream mode", font, BUTTON_STREAM);
        objects.push_back(myButton);

        buttonColor = getColor(Color::YELLOW);
        myButton = new Button(600, 300, 100, 50, buttonColor, true, "File", font, BUTTON_FILE);
        objects.push_back(myButton);
    }

    std::vector<Obj*> objects;
    char* readData();
    SDL_Window* window;
    SDL_Renderer* renderer;
    serialib serial;
    const char* port;
    int baudRate;
    bool quit;

};


// Main function
int main(int argc, char* args[]) {
    SDLSerialVisualizer visualizer("COM7", 9600);

    if (!visualizer.setup()) {
        std::cout << "Failed to initialize!\n";
        return -1;
    }

    visualizer.run();
    return 0;
}
