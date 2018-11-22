#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_net.h"
#include "SDL2/SDL_ttf.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
#include <cinttypes>
#include <vector>
#include <string>
#include <time.h>

#include "mechanics.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 90

using namespace std;

int modifier = 6;

int pixels = SCREEN_WIDTH * SCREEN_HEIGHT;
int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

SDL_Window     *window;
mechanics mech;
const Uint8 *state;
bool quit = false;

bool ipadd = true;
bool mainmenu = true;
string ip = "192.168.0.1";
short port = 9999;

void display();
void reshape_window(int, int);
void keyboardUp();
void keyboardDown();

int main( int argc, char* args[] ) 
{ 	
	srand(time(0));
	
	vector<string> sprites;
	vector<string> players;
	
	// mech.players.push_back("temp", (IPaddress) NULL); 
	// mech.players.push_back("temp", (IPaddress) NULL);
	// mech.players.push_back("temp", (IPaddress) NULL);
	// mech.players.push_back("temp", (IPaddress) NULL);
	
	players.push_back("temp");
	players.push_back("temp");
	players.push_back("temp");
	players.push_back("temp");
	
	sprites.push_back("assets/ui/backstraight.png"); //0
	sprites.push_back("assets/ui/countframe.png"); //1
	
	for(int i = 0; i < players.size(); i++) {
		sprites.push_back("assets/players/player_" + players[i] + ".png"); //wolfy 4, bray 5, zaw, 6, pixel 7
	}
	
	
    SDL_Renderer* renderer;
	SDL_Event       event;
	int w = 160;
	int h = 90;
	
	// uint32_t *background = new uint32_t[pixels];
	
	if(SDL_Init(SDL_INIT_VIDEO) == 0) {
		if(SDLNet_Init() == 1) {
			cout << "u try to init but garbo x d";
		}
	}
	else {
		cout << "lmao ur shit didn't init";
	}

	window = SDL_CreateWindow("Pixel UNO", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0); 
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	vector<SDL_Texture*> textures(9);
	for (int i = 1; i < sprites.size() + 1; i++) {
        textures[i] = IMG_LoadTexture(renderer, sprites[i - 1].c_str());
        SDL_QueryTexture(textures[i], NULL, NULL, &w, &h);
    }
	
	textures[9] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH , SCREEN_HEIGHT);
	
	for (int i = 0; i < sprites.size() - 2; i++) {
		SDL_SetTextureBlendMode(textures[i], SDL_BLENDMODE_BLEND);
	}
	
	//this_thread::sleep_for(chrono::seconds(0));
	
	mech.init(w, h, renderer);
	
	reshape_window(display_width, display_height);
	
	vector<SDL_Rect> rects;
	rects.push_back(SDL_Rect{4, 3, 10, 14}); //backLeftOppon						0
	rects.push_back(SDL_Rect{134, 3, 10, 14});  //backRightOppon				1
	rects.push_back(SDL_Rect{61, 2, 10, 14});  //backTopOppon					2
	rects.push_back(SDL_Rect{68, 38, 10, 14});  //backDeck							3
	rects.push_back(SDL_Rect{18, 23, 11, 11});  //leftOpponCountFrame		4
	rects.push_back(SDL_Rect{89, 6, 11, 11});  //topOpponCountFrame			5
	rects.push_back(SDL_Rect{148, 23, 11, 11});  //rightOpponCountFrame	6
	rects.push_back(SDL_Rect{18, 78, 11, 11});  //playerCountFrame				7
	rects.push_back(SDL_Rect{1, 18, 16, 16});  //leftOpponPic						8
	rects.push_back(SDL_Rect{72, 1, 16, 16});  //topOpponPic						9
	rects.push_back(SDL_Rect{131, 18, 16, 16});  //rightOpponPic					10
	rects.push_back(SDL_Rect{1, 73, 16, 16});  //playerPic							11
	
	SDL_Surface* temp = IMG_Load("assets/ui/numbers.png");
	SDL_Texture* spriteSheet = SDL_CreateTextureFromSurface(renderer, temp);
	SDL_FreeSurface(temp);
	SDL_Rect windowRect = {0, 0, 5, 6};
	SDL_Rect textureRect = {0, 0, 0, 0};
	SDL_QueryTexture(spriteSheet, NULL, NULL, &textureRect.w, &textureRect.h);
	textureRect.w /= 10;
	textureRect.h /= 2;
	
	//game loop
	while (!quit)
	{				
		while( SDL_PollEvent( &event ) ){
		/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
			switch( event.type ){
			  case SDL_KEYDOWN:
				state = SDL_GetKeyboardState(NULL);
				keyboardDown();
				break;

			  case SDL_KEYUP:
				keyboardUp();
				break;

			  default:
				break;
			}
		}
				
		SDL_SetRenderTarget(renderer, textures[9]);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
		SDL_Rect uwu = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
		SDL_RenderFillRect(renderer, &uwu);
		
		if(mainmenu) {
			//text			
			for(int i = 0; i < ip.size(); i++) {
				if(ip[i] != '.') {
					textureRect.x = ip[i] - '0';
					textureRect.x *= 5;
					textureRect.y = 0;
					textureRect.w = 5;
					windowRect.w = 5;
				}
				else {
					textureRect.x = 40;
					textureRect.y = 6;
					textureRect.w = 2;
					windowRect.w = 2;
				}
				
				SDL_RenderCopy(renderer, spriteSheet, &textureRect, &windowRect);
				if(ip[i] == '.')
						windowRect.x += 2;
					else
						windowRect.x += 5;
			}
			//button
			
			windowRect.x = 0;
		}
		
		else {
			SDL_RenderCopy(renderer, textures[1], NULL, &rects[0]); 
			SDL_RenderCopy(renderer, textures[1], NULL, &rects[1]);
			SDL_RenderCopy(renderer, textures[1], NULL, &rects[2]);
			SDL_RenderCopy(renderer, textures[1], NULL, &rects[3]);
			SDL_RenderCopy(renderer, textures[2], NULL, &rects[4]);
			SDL_RenderCopy(renderer, textures[2], NULL, &rects[5]);
			SDL_RenderCopy(renderer, textures[2], NULL, &rects[6]);
			SDL_RenderCopy(renderer, textures[2], NULL, &rects[7]);
			SDL_RenderCopy(renderer, textures[3], NULL, &rects[8]);
			SDL_RenderCopy(renderer, textures[4], NULL, &rects[9]);
			SDL_RenderCopy(renderer, textures[5], NULL, &rects[10]);
			SDL_RenderCopy(renderer, textures[6], NULL, &rects[11]);

			mech.tick(renderer);
		}
		
		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, textures[9], NULL, NULL);
		SDL_RenderPresent(renderer);
	}
	
	for (int i = 0; i < textures.size(); i++) {
		SDL_DestroyTexture(textures[i]);
	}
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window); 
	SDL_Quit(); 

	return 0;   
}

void reshape_window(int w, int h) {
	SDL_SetWindowSize(window, w, h);
}

void keyboardDown()
{	
	if(state[SDL_SCANCODE_ESCAPE])    {
		quit = true;
	}
	
	if(ipadd) {
		if(state[SDL_SCANCODE_0]) {
			ip += "0";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_1]) {
			ip += "1";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_2]) {
			ip += "2";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_3]) {
			ip += "3";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_4]) {
			ip += "4";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_5]) {
			ip += "5";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_6]) {
			ip += "6";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_7]) {
			ip += "7";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_8]) {
			ip += "8";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_9]) {
			ip += "9";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_PERIOD]) {
			ip += ".";
			ipadd = false;
		}
		if(state[SDL_SCANCODE_BACKSPACE]) {
			if(ip.size() > 0) {
				ip.pop_back();
			}
			
			ipadd = false;
		}
	}

	if(state[SDL_SCANCODE_A]) {
	    mech.pressedA = true;
	}
	if(state[SDL_SCANCODE_D]) {
		mech.pressedD = true;
	}
	if(state[SDL_SCANCODE_E]) {
		mech.pressedE = true;
	}
	if(state[SDL_SCANCODE_R]) {
		mech.pressedR = true;
	}
	if(state[SDL_SCANCODE_G]) {
		mech.pressedG = true;
	}
	if(state[SDL_SCANCODE_B]) {
		mech.pressedB = true;
	}
	if(state[SDL_SCANCODE_Y]) {
		mech.pressedY = true;
	}
	if(state[SDL_SCANCODE_SPACE]) {
		mech.pressedSpace = true;
	}
	if(state[SDL_SCANCODE_RETURN]) {
		mainmenu = false;
		ipadd = false;
	}
}

void keyboardUp()
{	
	if(mainmenu) {
		if(!(state[SDL_SCANCODE_0])){
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_1])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_2])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_3])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_4])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_5])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_6])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_7])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_8])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_9])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_PERIOD])) {
			ipadd = true;
		}
		if(!(state[SDL_SCANCODE_BACKSPACE])) {
			ipadd = true;
		}
	}
		
	if(!(state[SDL_SCANCODE_A])) {
		if(mech.pressedA == true) {
			mech.pressedA = false;
			mech.changed = false;
		}
	}
	if(!(state[SDL_SCANCODE_D])) {
		if(mech.pressedD == true) {
			mech.pressedD = false;
			mech.changed = false;
		}
	}
	if(!(state[SDL_SCANCODE_E])) {
		if(mech.pressedE == true) {
			mech.pressedE = false;
			mech.changed = false;
		}
	}
	if(!(state[SDL_SCANCODE_R])) {
		if(mech.pressedR == true) {
			mech.pressedR = false;
			mech.changed = false;
		}
	}
	if(!(state[SDL_SCANCODE_G])) {
		if(mech.pressedG == true) {
			mech.pressedG = false;
			mech.changed = false;
		}
	}
	if(!(state[SDL_SCANCODE_B])) {
		if(mech.pressedB == true) {
			mech.pressedB = false;
			mech.changed = false;
		}
	}
	if(!(state[SDL_SCANCODE_Y])) {
		if(mech.pressedY == true) {
			mech.pressedY = false;
			mech.changed = false;
		}
	}
	if(!(state[SDL_SCANCODE_SPACE])) {
		if(mech.pressedSpace == true) {
			mech.pressedSpace = false;
			mech.changed = false;
		}
	}
}

