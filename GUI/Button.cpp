#define _CRT_SECURE_NO_WARNINGS
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


int Button::readData(void* data, int dataSize) {
    if (data == nullptr || dataSize <= 0) {
        std::cerr << "Invalid data pointer or data size!" << std::endl;
        return -1;
    }

    char* buffer = static_cast<char*>(data);
    int bytesRead = 0;
    int totalBytesRead = 0;

    // Get the total size of the file
    long fileSize = _filelength(fd);
    if (fileSize == -1) {
        std::cerr << "Error getting file size: " << strerror(errno) << std::endl;
        return -1;
    }

    std::cout << "File size: " << fileSize << " bytes" << std::endl;

    // Read in chunks until we've read the requested number of bytes or reach EOF
    while (totalBytesRead < dataSize) {
        bytesRead = _read(fd, buffer + totalBytesRead, dataSize - totalBytesRead);
        std::cout << "bytesRead= " << bytesRead << std::endl;

        if (bytesRead <= 0) {
            // Check current position in the file using _lseek
            long currentPos = _lseek(fd, 0, SEEK_CUR);
            std::cout << "Current file position: " << currentPos << " bytes" << std::endl;

            // Check if we've reached the end of the file
            if (bytesRead == 0) {
                std::cout << "End of file reached." << std::endl;
                break;  // Stop if we've reached EOF
            }
            else {
                // Error occurred during reading
                char errorMsg[256];
                strerror_s(errorMsg, sizeof(errorMsg), errno);
                std::cerr << "Failed to read data. Error: " << errorMsg << std::endl;
                return -1;
            }
        }

        totalBytesRead += bytesRead;

        // After reading, manually adjust the file pointer back to where we stopped if not all data is read
        if (totalBytesRead < dataSize) {
            long currentPos = _lseek(fd, 0, SEEK_CUR); // Get current file position
            std::cout << "After reading, current file position: " << currentPos << " bytes" << std::endl;

            // Adjust file pointer back to the last position
            _lseek(fd, currentPos - bytesRead, SEEK_SET);  // Reset the pointer to the last read position
        }

        // If we've read more bytes than the file contains, we stop the loop
        if (totalBytesRead > fileSize) {
            std::cerr << "Attempted to read past the end of the file!" << std::endl;
            return -1;
        }
    }

    // Check if we successfully read the requested data size
    if (totalBytesRead != dataSize) {
        std::cerr << "Warning: Requested to read " << dataSize << " bytes, but only read " << totalBytesRead << " bytes!" << std::endl;
    }

    std::cout << "Successfully read " << totalBytesRead << " bytes." << std::endl;
    return totalBytesRead;
}


Button8Bit::Button8Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font,  SDLVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font), myVisualizer(visualizer) {}

void Button8Bit::onClick() {
    uint8_t receivedNumber = 0;
    std::cout << "8 Bit Button clicked!" << std::endl;

    int dataSize = 8;
    int res = readData(&receivedNumber, sizeof receivedNumber);


    std::vector<Obj*> objects = myVisualizer->getObjects();
    for (Obj* obj : objects) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateNumber(receivedNumber, 8);
        }
    }
}

void Button8Bit::process() {}


Button16Bit::Button16Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font), myVisualizer(visualizer) {}

void Button16Bit::onClick() {
    uint16_t receivedNumber = 0;
    std::cout << "16 Bit Button clicked!" << std::endl;

    int res = readData(&receivedNumber, sizeof receivedNumber);


    std::vector<Obj*> objects = myVisualizer->getObjects();
    for (Obj* obj : objects) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateNumber(receivedNumber, 16);
        }
    }
}

void Button16Bit::process() {}


Button32Bit::Button32Bit(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font), myVisualizer(visualizer) {}

void Button32Bit::onClick() {
    uint32_t receivedNumber = 0;
    std::cout << "32 Bit Button clicked!" << std::endl;

    int res = readData(&receivedNumber, sizeof receivedNumber);

    std::vector<Obj*> objects = myVisualizer->getObjects();
    for (Obj* obj : objects) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateNumber(receivedNumber, 32);
        }
    }
}

void Button32Bit::process() {}


ButtonBitMap::ButtonBitMap(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font),
    myVisualizer(visualizer){

    buffer = new uint8_t[BITMAP_SIZE];
}

ButtonBitMap::~ButtonBitMap() {
    delete[] buffer;
}

void ButtonBitMap::onClick() {
    active = true;
    setColor(getColor(Color::GRAY6));


    int bytesRead = readData(buffer, BITMAP_SIZE);
    std::cout << "Button activated: Reading " << bytesRead << " bytes from file descriptor." << std::endl;
    if (bytesRead != (BITMAP_SIZE)) {
        std::cerr << "Failed to read"<< BITMAP_SIZE << "bytes! Bytes read: " << bytesRead << std::endl;
        active = false;
        setColor(getColor(Color::GRAY3));
        return;
    }

    std::vector<Obj*> objects = myVisualizer->getObjects();
    for (Obj* obj : objects) {
        NumberDisplay* numberDisplay = dynamic_cast<NumberDisplay*>(obj);
        if (numberDisplay) {
            numberDisplay->updateBitmap(buffer);
        }
    }

    std::cout << "Bitmap updated successfully for all NumberDisplay objects." << std::endl;
   
}

void ButtonBitMap::process() {
    active = false;
    setColor(getColor(Color::GRAY3));
}



ButtonFile::ButtonFile(int x, int y, int width, int height, SDL_Color color, bool available, std::string text, TTF_Font* font, SDLVisualizer* visualizer)
    : Button(x, y, width, height, color, available, text, font),
    myVisualizer(visualizer)
{
    file.open("serial_data", std::ios::out | std::ios::trunc);
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

    uint32_t value;

    int bytesRead = readData(&value, sizeof value);
    if (bytesRead == sizeof value) {

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


void ButtonFile::onClick() 
{
    active = !active;
    if (active)
        setColor(getColor(Color::GRAY3));
    else
        setColor(getColor(Color::GRAY6));
}