#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#include "SDL2/SDL_image.h"
#include <string>

using namespace std;

struct card {
	SDL_Texture* texture;
	string color;
	string type;
};