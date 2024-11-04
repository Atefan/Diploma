#include <iostream>
#include <bitset>
#include <SDL.h>
#include "serialib.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BORDER_SIZE 500


class Matrix {
public:
    Matrix(int width, int height) : width(width), height(height), last_index(0) {}

    void render(SDL_Renderer* renderer, char* data, int size) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //  Border
        int xOffset = (SCREEN_WIDTH - BORDER_SIZE) / 2;
        int yOffset = (SCREEN_HEIGHT - BORDER_SIZE) / 2;
        SDL_Rect borderRect = { xOffset, yOffset, BORDER_SIZE, BORDER_SIZE };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &borderRect);
        SDL_SetRenderDrawColor(renderer,0, 0,255, 255);
        SDL_RenderDrawPoint(renderer, xOffset, yOffset);

        int squareSize = 1;
        int numCols = BORDER_SIZE;
        int numRows = BORDER_SIZE;

        int startX = xOffset;
        int startY = yOffset;

        int bitIndex = 0;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < 8; ++j) {
                // Get the current bit
                bool bit = (data[i] >> j) & 1;

                // Calculate the position in the border area
                int row = (bitIndex / numCols);
                int col = (bitIndex % numCols);

                // Ensure we don't go out of bounds
                if (row >= numRows || col >= numCols) {
                    break; // If out of bounds, break
                }

                // Set the color based on the bit value
                SDL_SetRenderDrawColor(renderer, bit ? 255 : 0, 0, bit ? 0 : 255, 255);  // Red for 1, Blue for 0

                // Draw the pixel
                SDL_RenderDrawPoint(renderer, startX + col, startY + row);

                ++bitIndex;

                // Stop if we have rendered enough bits
                if (bitIndex >= numCols * numRows) {
                    break;
                }
            }
            // Stop if we have rendered enough bits
            if (bitIndex >= numCols * numRows) {
                break;
            }
        }

        // Update the screen
        SDL_RenderPresent(renderer);
    }


private:
    int width;
    int height;
    int last_index;
    int matrix[8]; // Array for storing bit values
};
class SDLSerialVisualizer {
public:
    SDLSerialVisualizer(const char* port, int baudRate)
        : port(port), baudRate(baudRate), window(nullptr), renderer(nullptr), quit(false), matrix(8, 8) {}

    ~SDLSerialVisualizer() {
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

        window = SDL_CreateWindow("COM Port Data Visualization",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
            return false;
        }

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
            }
            int dataSize = 0;
            char* data = readData(&dataSize);

            if (data != nullptr) {
                matrix.render(renderer, data, dataSize);
                delete[] data;  // Free allocated memory after rendering
            }
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
