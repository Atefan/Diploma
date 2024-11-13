#include "ColorUtils.h"

SDL_Color getColor(Color color) {
    switch (color) {
    case Color::RED:      return { 255, 0, 0, 255 };
    case Color::GREEN:    return { 0, 255, 0, 255 };
    case Color::BLUE:     return { 0, 0, 255, 255 };
    case Color::WHITE:    return { 255, 255, 255, 255 };
    case Color::BLACK:    return { 0, 0, 0, 255 };
    case Color::YELLOW:   return { 255, 255, 0, 255 };
    case Color::CYAN:     return { 0, 255, 255, 255 };
    case Color::MAGENTA:  return { 255, 0, 255, 255 };
    default:              return { 0, 0, 0, 255 }; // Default to black if unknown
    }
}
