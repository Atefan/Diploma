#include <iostream>
#include <bitset>
#include <SDL.h>
#include "serialib.h"
#include <SDL_ttf.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define BORDER_SIZE 700

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


class Button {
public:
    Button(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font)
        : x(x), y(y), width(width), height(height), color(color), available(available), text(text), font(font) {}

    void render(SDL_Renderer* renderer) {
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

    void handleEvent(SDL_Event* event) {
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
        std::cout << "Button clicked!" << std::endl;
    }

private:
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
        : port(port), baudRate(baudRate), window(nullptr), renderer(nullptr), quit(false), matrix(8, 8), myButton(nullptr) {}

    ~SDLSerialVisualizer() {
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

        SDL_Color buttonColor = { 0, 0, 255, 255 };
        myButton = new Button(200, 150, 100, 50, buttonColor, true, "Button 1", font);
        
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

                myButton->handleEvent(&e);
            }
            
            myButton->render(renderer);


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
    char* readData();
    SDL_Window* window;
    SDL_Renderer* renderer;
    serialib serial;
    const char* port;
    int baudRate;
    bool quit;

    Button *myButton;
    Matrix matrix;
};


// Main function
int main(int argc, char* args[]) {
    SDLSerialVisualizer visualizer("COM9", 9600);

    if (!visualizer.setup()) {
        std::cout << "Failed to initialize!\n";
        return -1;
    }

    visualizer.run();
    return 0;
}
