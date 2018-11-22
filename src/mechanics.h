#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_net.h"
#include <vector>
#include <string>
#include <iostream> 
#include <cstdlib> 
#include <sstream> 

#include "card.h"
#include "player.h"

using namespace std;

class mechanics {
public:

	int turn;
	int cardAmount;
	//simplify into "dictionary" list
	bool pressedA = false;
	bool pressedD = false;
	bool pressedSpace = false;
	bool pressedE = false;
	
	bool pressedR = false;
	bool pressedG = false;
	bool pressedB = false;
	bool pressedY = false;

	bool changed = false;
	bool gameOver = false;
	int startX = 36;
	SDL_Rect cardpile;
	
	
	static vector<string> specialTypes;
	static vector<SDL_Rect> locations;
	static vector<string> cardSprites;
	static vector<card> cards;
	static vector<card> hand;
	static int selectedPos;
	static card topCard;
	static int setupCardsLocation;
	static vector<string> order;
	static vector<player> players;
	
	void init(int, int, SDL_Renderer*);
	void tick(SDL_Renderer*);
	void setupCards(string, string);
	void cardMoving(int);
	int stayInBounds(int);
	int addBoundBox(int, int, int, int);
	int changeTurns(int);
	string lazyColorCheck();
	vector<card> sortCardArray(vector<card>);
};