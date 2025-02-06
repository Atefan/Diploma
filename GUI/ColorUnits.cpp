#include "ColorUtils.h"

SDL_Color getColor(Color color) {
    switch (color) {
    case Color::BLACK:   return { 0, 0, 0, 255 };
    case Color::GRAY1:   return { 28, 28, 28, 255 };
    case Color::GRAY2:   return { 56, 56, 56, 255 };
    case Color::GRAY3:   return { 84, 84, 84, 255 };
    case Color::GRAY4:   return { 112, 112, 112, 255 };
    case Color::GRAY5:   return { 140, 140, 140, 255 };
    case Color::GRAY6:   return { 168, 168, 168, 255 };
    case Color::GRAY7:   return { 196, 196, 196, 255 };
    case Color::GRAY8:   return { 224, 224, 224, 255 };
    case Color::GRAY9:   return { 240, 240, 240, 255 };
    case Color::WHITE:   return { 255, 255, 255, 255 };
    default:             return { 0, 0, 0, 255 };
    }
}
