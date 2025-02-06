#include "NumberDisplay.h"


NumberDisplay::NumberDisplay(int x, int y, int width, int height, Color color, TTF_Font* font)
    : x(x), y(y), width(width), height(height), color(getColor(color)), font(font), number(55), bits(32) {}

void NumberDisplay::updateNumber(uint32_t newNumber, int newBits) {
    number = newNumber;
    bits = newBits;
    renderBitmap = false;
}

void NumberDisplay::updateBitmap(uint8_t* newBitmap) {
    if (newBitmap == nullptr) {
        std::cerr << "Invalid bitmap data!" << std::endl;
        return;
    }

    memcpy(bitmap, newBitmap, BITMAP_SIZE);
    renderBitmap = true;
}

void NumberDisplay::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = { x, y, width, height };
    SDL_RenderFillRect(renderer, &rect);

    
    if (renderBitmap) {
        int bitsPerRow = sqrt(BITMAP_SIZE);
        int boxSize = 1;

        int bitmap_x = x + width / 2 - bitsPerRow * boxSize / 2;
        int bitmap_y = y;

        for (int i = 0; i < BITMAP_SIZE; ++i) {
            int bitValue = (bitmap[i / 8] >> (7 - i % 8)) & 1;

            int row = i / bitsPerRow;
            int col = i % bitsPerRow;

            int xPos = bitmap_x + col * boxSize;
            int yPos = bitmap_y + row * boxSize;

            SDL_Color boxColor = (bitValue == 1) ? getColor(Color::BLACK) : getColor(Color::WHITE);

            SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
            SDL_Rect boxRect = { xPos, yPos, boxSize, boxSize };
            SDL_RenderFillRect(renderer, &boxRect);
        }
    }
    else
    {
        std::string binaryStr;
        std::string numberStr;
        num_to_string(binaryStr, numberStr, number);

        SDL_Color textColor = getColor(Color::BLACK);

        SDL_Surface* decimalSurface = TTF_RenderText_Blended(font, numberStr.c_str(), textColor);
        if (decimalSurface) {
            SDL_Texture* decimalTexture = SDL_CreateTextureFromSurface(renderer, decimalSurface);
            int textWidth = decimalSurface->w;
            int textHeight = decimalSurface->h;
            SDL_FreeSurface(decimalSurface);

            SDL_Rect decimalRect = { x + (width - textWidth) / 2, y + (height / 4) - (textHeight / 2), textWidth, textHeight };
            SDL_RenderCopy(renderer, decimalTexture, NULL, &decimalRect);
            SDL_DestroyTexture(decimalTexture);
        }

        SDL_Surface* binarySurface = TTF_RenderText_Blended(font, binaryStr.c_str(), textColor);
        if (binarySurface) {
            SDL_Texture* binaryTexture = SDL_CreateTextureFromSurface(renderer, binarySurface);
            int textWidth = binarySurface->w;
            int textHeight = binarySurface->h;
            SDL_FreeSurface(binarySurface);

            SDL_Rect binaryRect = { x + (width - textWidth) / 2, y + (3 * height / 4) - (textHeight / 2), textWidth, textHeight };
            SDL_RenderCopy(renderer, binaryTexture, NULL, &binaryRect);
            SDL_DestroyTexture(binaryTexture);
        }

    }
    
}

void NumberDisplay::handleEvent(SDL_Event* event) {}

void NumberDisplay::process() {}

void NumberDisplay::num_to_string(std::string& binaryStr, std::string& numberStr, uint32_t number) {
    binaryStr = "";
    for (int i = bits - 1; i >= 0; --i) {
        if ((i + 1) % 4 == 0)
            binaryStr += ' ';
        binaryStr += ((number >> i) & 1) ? '1' : '0';
    }

    std::ostringstream oss;
    oss << number;
    numberStr = oss.str();
}
