#include <iostream>
#include <sstream>
#include <bitset>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>

#include "Button.h"
#include "Constants.h"
#include "NumberDisplay.h"
#include "Obj.h"
#include "SDLVisualizer.h"


int main(int argc, char* args[]) {
	SDLVisualizer visualizer;
	if (!visualizer.setup()) {
		std::cout << "Failed to initialize!\n";
		return -1;
	}

	visualizer.run();
	return 0;
}
