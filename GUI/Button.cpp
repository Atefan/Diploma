#include "Button.h"


Button::Button(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font)
    : x(x), y(y), width(width), height(height), color(color), active(available), text(text), font(font) {}

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

Button8Bit::Button8Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font,  SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font), myVisualizer(visualizer) {}

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
            numberDisplay->updateNumber(receivedNumber, 8);
        }
    }
}

void Button8Bit::process() {}


Button16Bit::Button16Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font), myVisualizer(visualizer) {}

void Button16Bit::onClick() {
    uint16_t receivedNumber = 0;
    std::cout << "16 Bit Button clicked!" << std::endl;

    const char* sendData = "1";
    serial.writeBytes(sendData, strlen(sendData));

    int dataSize = 16;
    receivedNumber = readData(dataSize);


    std::vector<Obj*> objects = myVisualizer->getObjects();
    for (Obj* obj : objects) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateNumber(receivedNumber, 16);
        }
    }
}

void Button16Bit::process() {}


Button32Bit::Button32Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font), myVisualizer(visualizer) {}

void Button32Bit::onClick() {
    uint32_t receivedNumber = 0;
    std::cout << "32 Bit Button clicked!" << std::endl;

    const char* sendData = "3";
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


ButtonStreamBit::ButtonStreamBit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font),
    myVisualizer(visualizer), bufferIndex(0), currentSize(0), bufferSize(64) {
    buffer = new uint32_t[bufferSize];
}


ButtonStreamBit::~ButtonStreamBit() {
    delete[] buffer;
}

void ButtonStreamBit::process() {
    if (!active) return;

    if (bufferIndex >= currentSize) {
        currentSize = serial.readBytes(reinterpret_cast<char*>(buffer), bufferSize * sizeof(int), 1000);
        bufferIndex = 0;

        if (currentSize <= 0) return;

        currentSize /= sizeof(int);
    }

    uint32_t nextValue = buffer[bufferIndex++];

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
        serial.writeBytes(deactivateSignal, strlen(deactivateSignal));
        active = false;
        setColor(getColor(Color::GRAY3));
    }
    else {
        const char* activateSignal = "4";
        serial.writeBytes(activateSignal, strlen(activateSignal));
        active = true;
        setColor(getColor(Color::GRAY6));
    }
}



ButtonFile::ButtonFile(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLSerialVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font),
    myVisualizer(visualizer)
{
    file.open("serial_data.txt", std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing!" << std::endl;
    }
}

ButtonFile::~ButtonFile() {
    if (file.is_open()) 
        file.close();
}

void ButtonFile::process() {
    if (!active || !file.is_open()) return;

    const int ARRAY_SIZE = sizeof(uint16_t);
    uint16_t value;

    int bytesRead = serial.readBytes(&value, ARRAY_SIZE, 1000);
    if (bytesRead == ARRAY_SIZE) {

        file << value << std::endl;
        std::cout << "Value written to file: " << value << std::endl;

        std::vector<Obj*> objects = myVisualizer->getObjects();
        for (Obj* obj : objects) {
            NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
            if (numberDisplay) {
                numberDisplay->updateNumber(value, 32);
            }
        }
    }
}


void ButtonFile::onClick() {

    const char* Signal = "5";
    serial.writeBytes(Signal, strlen(Signal));
    active = !active;
    if (active)
        setColor(getColor(Color::GRAY3));
    else
        setColor(getColor(Color::GRAY6));
}