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
    MAGENTA,
    GRAY
};

SDL_Color getColor(Color color);
