#include "Button.h"
#include "SDLSerialVisualizer.h"
#include "ColorUtils.h"

Button::Button(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id)
    : x(x), y(y), width(width), height(height), color(color), available(available), text(text), font(font), id(id) {}

void Button::render(SDL_Renderer* renderer) {
    if (!available) return;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect buttonRect = { x, y, width, height };
    SDL_RenderFillRect(renderer, &buttonRect);

    SDL_Color textColor = { 0, 0, 0, 255 }; // Black color for text
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

void Button::handleEvent(SDL_Event* event) {
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

void Button::setColor(SDL_Color newColor) {
    color = newColor;
}

void Button::setAvailable(bool isAvailable) {
    available = isAvailable;
}

char* Button::readData(int* dataSize) {
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

Button8Bit::Button8Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id, SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font, id), myVisualizer(visualizer) {}

void Button8Bit::onClick() {
    uint8_t receivedNumber = 0;
    std::cout << "8 Bit Button clicked!" << std::endl;

    const char* sendData = "0";
    serial.writeBytes(sendData, strlen(sendData));

    int dataSize = 0;
    char* data = readData(&dataSize);

    if (dataSize > 0) {
        try {
            receivedNumber = std::stoi(data);  // Convert to integer if response is numeric
            std::cout << "Received integer: " << receivedNumber << std::endl;
        }
        catch (std::invalid_argument& e) {
            std::cout << "Invalid data received: " << data << std::endl;
        }
    }
    else {
        std::cout << "No data received or data size is zero." << std::endl;
    }

    delete[] data;
    for (Obj* obj : myVisualizer->getObjects()) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateNumber(receivedNumber, 8);  // Update the display with the new number
        }
    }
}

Button16Bit::Button16Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id)
    : Button(x, y, width, height, color, available, text, font, id) {}

void Button16Bit::onClick() {
    std::cout << "16 Bit Button clicked!" << std::endl;
}

Button32Bit::Button32Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id)
    : Button(x, y, width, height, color, available, text, font, id) {}

void Button32Bit::onClick() {
    std::cout << "32 Bit Button clicked!" << std::endl;
}
