#include "mechanics.h"
	vector<string> mechanics::specialTypes{"plus2", "reverse", "skip"};
	vector<SDL_Rect> mechanics::locations;
	vector<string> mechanics::cardSprites;
	vector<card> mechanics::cards(54);
	vector<card> mechanics::hand;
	card mechanics::topCard;
	int mechanics::setupCardsLocation(0);
	int mechanics::selectedPos(0);
	vector<string> mechanics::order{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "plus2", "reverse", "skip", "", "plus4"};
	vector<player> mechanics::players;
	
void mechanics::init(int w, int h, SDL_Renderer* renderer) {	

	startX = 36; 
	
	for(int i = 0; i < 7; i++) {
		startX = addBoundBox(startX, 75, 10, 14);
	}
	
	turn = 1;
	cardAmount = 7;
	cardpile = {82, 38, 10, 14};
	locations[0].y = 71;


	//numbers
	for(int i = 0; i < 10; i++) { //r0, y0, b0, g0, r1, y1...
		setupCards("assets/cards/red", to_string(i)); 
		setupCards("assets/cards/green", to_string(i)); 
		setupCards("assets/cards/blue", to_string(i)); 
		setupCards("assets/cards/yellow", to_string(i));
	}
	//specials
	for(int i = 0; i < specialTypes.size(); i++) { //r+2, y+2, b+2, g+2, rr, yr...
		setupCards("assets/cards/red", specialTypes[i]); 
		setupCards("assets/cards/green", specialTypes[i]);
		setupCards("assets/cards/blue", specialTypes[i]);
		setupCards("assets/cards/yellow", specialTypes[i]);
	}
	//wilds
	setupCards("assets/cards/wild", ""); 
	setupCards("assets/cards/wild", "plus4"); 
	
	for (int i = 0; i < cardSprites.size(); i++) {
        cards[i].texture = IMG_LoadTexture(renderer, cardSprites[i].c_str());
	    SDL_QueryTexture(cards[i].texture, NULL, NULL, &w, &h);
    }
	
	for (int i = 0; i < cardSprites.size(); i++) {
		SDL_SetTextureBlendMode(cards[i].texture, SDL_BLENDMODE_BLEND);
	}
	for(int i = 0; i < 7; i++) {
		hand.push_back(cards[(rand() % 54)]);
	}
	
	hand = sortCardArray(hand);
	topCard = cards[(rand() % 54)];
}

void mechanics::tick(SDL_Renderer* renderer) {	
	if(!gameOver) {
		//change selected
		if(topCard.color == "wild") {
			topCard.color = lazyColorCheck();
		}
		else if(pressedA == true && changed == false) {
			cardMoving(0);
			
			changed = true;
		}
		else if(pressedD == true && changed == false) {
			cardMoving(1);
			
			changed = true;
		}
		else if(pressedSpace == true && changed == false) {
			//to do: place cards code and automatically adjust hand to look natural
			if(hand.size() > 0) {
				cardMoving(2);
			}
			
			changed = true;
		}
		else if(pressedE == true && changed == false) {
			cardMoving(3);
			hand = sortCardArray(hand);
			
			changed = true;
		}
		else {
			//do nothing
		}
	}
	
	//render cards
	for(int i = 0; i < hand.size(); i++) {
		SDL_RenderCopy(renderer, hand[i].texture, NULL, &locations[i]);
	}
		
	SDL_RenderCopy(renderer, topCard.texture, NULL, &cardpile);
}

void mechanics::setupCards(string locationC, string number) {
	cardSprites.push_back(locationC + number + ".png");
	cards[setupCardsLocation].color = locationC.substr(13, locationC.length()); //13 is assuming the location is assets/cards/, could use cards/ if player wants to move it to a deeper subfolder or just bake it in
	cards[setupCardsLocation].type = number;
	setupCardsLocation++;
}

void mechanics::cardMoving(int express) {
	switch(express) {
		case(0): //A
			locations[selectedPos].y = 75;
			selectedPos--;
			selectedPos = stayInBounds(selectedPos);
			locations[selectedPos].y = 71;
			//add flashing box around card
			break;
			
		case(1): //D
			locations[selectedPos].y = 75;
			selectedPos++;
			selectedPos = stayInBounds(selectedPos);
			locations[selectedPos].y = 71;
			//add flashing box around card
			break;
			
		case(2): //space
			if(hand[selectedPos].color == topCard.color || hand[selectedPos].type == topCard.type || hand[selectedPos].color == "wild") {
			
				
				topCard = hand[selectedPos];
				
				hand.erase(hand.begin() + selectedPos);
				locations.erase(locations.end() - 1);
				
				if(selectedPos == hand.size())
					selectedPos--;
				
				locations[selectedPos].y = 71;
				
				startX -= 12;
			}
			
			else {
				cout << hand[selectedPos].color;
			}
			break;
			
		case(3): //E
			hand.push_back(cards[(rand() % 54)]);
			
			//if(hand.size() >= 8) {
				//to do
			//}
			//else {
				locations[selectedPos].y = 75;
				selectedPos = hand.size() - 1;
				selectedPos = stayInBounds(selectedPos);
				
				startX = addBoundBox(startX, 71, 10, 14); //IM DUMB LOL
			//}
			
			//add flashing box around card
		
			break;
			
		default:
			cout << "nothing";
			break;
	}
}

int mechanics::stayInBounds(int sel) {
	// make sure pos is inbounds
	if(sel < 0) {
		sel = hand.size() - 1;
	}
	else if(sel >= hand.size()) {
		sel = 0;
	}
	else {
		// do nothing
	}
	
	return sel;
}

int mechanics::addBoundBox(int x, int y, int w, int h) {
	locations.push_back(SDL_Rect{x, y, w, h});
	return x + 12;
}

int mechanics::changeTurns(int turn) {
	if(turn == 4) {
		return 1;
	}
	
	return turn + 1;
}

string mechanics::lazyColorCheck() {
	if(pressedR == true) {
			return "red";
	}
	else if(pressedG == true) {
			return "green";
	}
	else if(pressedB == true) {
			return "blue";
	}
	else if(pressedY == true) {
			return "yellow";
	}
	
	return "wild";
}

vector<card> mechanics::sortCardArray(vector<card> hand) {
	vector<card> temp;
	
	//red
	for(int j = 0; j < order.size(); j++) { 
		for(int i = 0; i < hand.size(); i++) {
			if(hand[i].color == "red") { 	
				if(hand[i].type == order[j]) {
					temp.push_back(hand[i]);
				}
			}
		}
	}
	
	//green
	for(int j = 0; j < order.size(); j++) { 
		for(int i = 0; i < hand.size(); i++) {
			if(hand[i].color == "green") { 	
				if(hand[i].type == order[j]) {
					temp.push_back(hand[i]);
				}
			}
		}
	}
	
	//blue
	for(int j = 0; j < order.size(); j++) { 
		for(int i = 0; i < hand.size(); i++) {
			if(hand[i].color == "blue") { 	
				if(hand[i].type == order[j]) {
					temp.push_back(hand[i]);
				}
			}
		}
	}
	
	//yellow
	for(int j = 0; j < order.size(); j++) { 
		for(int i = 0; i < hand.size(); i++) {
			if(hand[i].color == "yellow") { 	
				if(hand[i].type == order[j]) {
					temp.push_back(hand[i]);
				}
			}
		}
	}
	
	//wild
	for(int j = 0; j < order.size(); j++) { 
		for(int i = 0; i < hand.size(); i++) {
			if(hand[i].color == "wild") { 	
				if(hand[i].type == order[j]) {
					temp.push_back(hand[i]);
				}
			}
		}
	}
	
	return temp;
}
