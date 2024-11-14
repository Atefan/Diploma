#include "Button.h"


Button::Button(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id)
    : x(x), y(y), width(width), height(height), color(color), active(available), text(text), font(font), id(id) {}

void Button::render(SDL_Renderer* renderer) {

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect buttonRect = { x, y, width, height };
    SDL_RenderFillRect(renderer, &buttonRect);

    SDL_Color textColor = getColor(Color::BLACK);
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
    active = isAvailable;
}

int Button::readData(int dataSize) {

    int bytesToRead = (dataSize + 7) / 8;
    char* data = new char[bytesToRead];
    int bytesRead = 0;

    do {
        bytesRead = serial.readBytes(data, bytesToRead, 1000);
    } while (bytesRead <= 0);

    int result = 0;
    for (int i = 0; i < bytesRead; ++i)
        result |= (static_cast<unsigned char>(data[i]) << (8 * (bytesRead - 1 - i)));

    delete[] data;

    std::cout << "Data read (" << dataSize << " bits): " << result << std::endl;

    return result;
}

Button8Bit::Button8Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id, SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font, id), myVisualizer(visualizer) {}

void Button8Bit::onClick() {
    uint8_t receivedNumber = 0;
    std::cout << "8 Bit Button clicked!" << std::endl;

    const char* sendData = "0";
    serial.writeBytes(sendData, strlen(sendData));

    int dataSize = 8;
    receivedNumber = readData(dataSize);


    std::vector<Obj*> objects = myVisualizer->getObjects();
    for (Obj* obj : objects) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateNumber(receivedNumber, 8);  // Update the display with the new number
        }
    }
}

void Button8Bit::process() {}


Button16Bit::Button16Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id, SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font, id), myVisualizer(visualizer) {}

void Button16Bit::onClick() {
    uint16_t receivedNumber = 0;
    std::cout << "16 Bit Button clicked!" << std::endl;

    const char* sendData = "0";
    serial.writeBytes(sendData, strlen(sendData));

    int dataSize = 16;
    receivedNumber = readData(dataSize);


    std::vector<Obj*> objects = myVisualizer->getObjects();
    for (Obj* obj : objects) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateNumber(receivedNumber, 16);  // Update the display with the new number
        }
    }
}

void Button16Bit::process() {}


Button32Bit::Button32Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id, SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font, id), myVisualizer(visualizer) {}

void Button32Bit::onClick() {
    uint32_t receivedNumber = 0;
    std::cout << "32 Bit Button clicked!" << std::endl;

    const char* sendData = "0";
    serial.writeBytes(sendData, strlen(sendData));

    int dataSize = 32;
    receivedNumber = readData(dataSize);

    std::vector<Obj*> objects = myVisualizer->getObjects();
    for (Obj* obj : objects) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateNumber(receivedNumber, 32);
        }
    }
}

void Button32Bit::process() {}


ButtonStreamBit::ButtonStreamBit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, int id, SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font, id), myVisualizer(visualizer) {}

void ButtonStreamBit::process() {
    const int ARRAY_SIZE = 1 << 10;
    static int bufferSize = 64;
    static int buffer[ARRAY_SIZE];
    static int bufferIndex = 0;
    static bool bufferInitialized = false;

    if (!active) return;

    if (!bufferInitialized || bufferIndex >= bufferSize) {
        bufferSize = serial.readBytes(buffer, ARRAY_SIZE, 1000);

        bufferIndex = 0;
        bufferInitialized = true;
    }

    int nextValue = buffer[bufferIndex++];
    std::cout << "Next value from buffer: " << nextValue << std::endl;

    std::vector<Obj*> objects = myVisualizer->getObjects();
    for (Obj* obj : objects) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateNumber(nextValue, 32);
        }
    }
}

void ButtonStreamBit::onClick() {
    if (active) {

        const char* deactivateSignal = "8";
        serial.writeBytes(deactivateSignal, strlen(deactivateSignal)); // Send "8" to serial

        active = false;
        setColor(getColor(Color::GRAY)); // Set color to gray for inactive state
    }
    else {
        // Activate if currently inactive
        const char* activateSignal = "4";
        serial.writeBytes(activateSignal, strlen(activateSignal)); // Send "4" to serial

        active = true;
        setColor(getColor(Color::MAGENTA)); // Set color to magenta for active state
    }
}