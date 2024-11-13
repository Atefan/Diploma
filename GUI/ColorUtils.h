#pragma once

#include <SDL.h>

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

SDL_Color getColor(Color color);
