#pragma once

#include <SDL.h>

enum class Color {
    BLACK,
    GRAY1,
    GRAY2,
    GRAY3,
    GRAY4,
    GRAY5,
    GRAY6,
    GRAY7,
    GRAY8,
    GRAY9,
    WHITE
};

SDL_Color getColor(Color color);
