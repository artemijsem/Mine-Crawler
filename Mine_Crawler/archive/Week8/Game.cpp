#include "Game.h"
#include "TextureManager.h"
#include "GameObjects.h"
#include "Levels.h"
#include "AI.h"
#include<string>


//=================================================================================
// SDL
SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::playerInputEvent;

// Gobal Text Variables
TTF_Font* font = nullptr;
SDL_Color textColour = { 0, 0, 0 };
SDL_Surface* textSurface = nullptr;
SDL_Texture* textTexture = nullptr;
SDL_Rect textRect;

// Audio
Mix_Music* music = NULL;
Mix_Chunk* shootSound = NULL;

//==========================================
//Game Objects
PlayerInput playerInput;
Level* bg_map;
Level* map;
AI* gameAI;

// Create Squads
Squad* squadYellow;
Squad* squadRed;


// Game objects
Item* targetMarker;
Item* overlay;

Attack* bullets[100];
Item* hits[100]; // may need a new class

//=================================================================================
//Vars
int unitSelected = -1;



//=================================================================================
//Constructor
Game::Game()
{
	printf("Game Started \n");
}//---

//=================================================================================

void Game::createGameObjects()
{
	printf("************************* \nCreating GameObjects \n");

	bg_map = new Level();
	// Create Game Level Map
	map = new Level();

	// Create Teams (Max units = 50)
	
	squadYellow = new Squad("Yellow Team", 5, 10, 10); // name, tileType, start x y 
	squadYellow->createUnits(map, 5, 3); // level, spawn amount & state
	

	squadRed = new Squad("Red Team", 6, 30, 25); // name, tileType, start x y 
	squadRed->createUnits(map, 5, 3);
	

	// Create other Objects ------------------------------------------------

	// show mouse click pos with target Marker
	targetMarker = new Item("assets/images/Square_Purple.png", 16, 16, 8);


	// Sprite for Interface Overlay
	overlay = new Item("assets/images/Interface.png", 16, 16, 800);
	overlay->srcRect.w = 800;
	overlay->srcRect.h = 600;
	overlay->destRect.w = 800;
	overlay->destRect.h = 600;

}//---

//=================================================================================

void Game::welcomeScreen()
{

}//---

void Game::playAgainScreen()
{

}//---



//=================================================================================
void Game::findNearestTarget()
{
	// This is for red finding Yellow Targets

	// Get a list of the active Yellow units
	std::list<int> yelActiveUnits = squadYellow->getActiveUnitsIDs();
	std::list<int> redActiveUnits = squadRed->getActiveUnitsIDs();

	// Reset distance variables
	int nearestTarget = 0;
	float distanceToNearestUnit = 1000; //  a large number off the board initially

	// ensure a Red unit is selected
	if (unitSelected > 600 && unitSelected < 700)
	{
		// get the active Red unit's position
		Vector2 unitPos = squadRed->getUnitPosition(unitSelected - 601);

		// loop through active yellow units 
		for (int unit : yelActiveUnits)
		{
			// get loop unit Position
			Vector2 enemyUnitPos = squadYellow->getUnitPosition(unit - 501);

			// Calc distance using pythagoras
			float distance = sqrt((unitPos.x - enemyUnitPos.x) * (unitPos.x - enemyUnitPos.x) + (unitPos.y - enemyUnitPos.y) * (unitPos.y - enemyUnitPos.y));

			if (distance < distanceToNearestUnit) // check if this unit is closer
			{
				distanceToNearestUnit = distance;
				nearestTarget = unit;
			}
		}
		// Set Target of selected red unit
		squadRed->setUnitTarget(unitSelected - 601, nearestTarget);
		squadRed->setUnitTargetDistance(unitSelected - 601, distanceToNearestUnit);
	}


	if (unitSelected > 500 && unitSelected < 600)
	{
		// get the active Red unit's position
		Vector2 unitPos = squadYellow->getUnitPosition(unitSelected - 501);

		// loop through active yellow units 
		for (int unit : redActiveUnits)
		{
			// get loop unit Position
			Vector2 enemyUnitPos = squadRed->getUnitPosition(unit - 601);

			// Calc distance using pythagoras
			float distance = sqrt((unitPos.x - enemyUnitPos.x) * (unitPos.x - enemyUnitPos.x) + (unitPos.y - enemyUnitPos.y) * (unitPos.y - enemyUnitPos.y));

			if (distance < distanceToNearestUnit) // check if this unit is closer
			{
				distanceToNearestUnit = distance;
				nearestTarget = unit;
			}
		}
		// Set Target of selected red unit
		squadYellow->setUnitTarget(unitSelected - 501, nearestTarget);
		squadYellow->setUnitTargetDistance(unitSelected - 501, distanceToNearestUnit);
	}




	// Do red units have an attack Target in Range

	// Loop through all active Red units
	std::list<int> redUnitsActive = squadRed->getActiveUnitsIDs();
	for (int redUnit : redUnitsActive)
	{
		if (squadRed->getUnitTarget(redUnit - 601) > 0)
		{
			// get the target enemy ID 
			int enemyTargetID = squadRed->getUnitTarget(redUnit - 601);

			if (enemyTargetID > 500 && enemyTargetID < 600)
			{				
				Vector2 enemyUnitPos = squadYellow->getUnitPosition(enemyTargetID - 501);
				Vector2 unitPos = squadRed->getUnitPosition(redUnit - 601);

				// Calc distance using pythagoras
				float distance = sqrt((unitPos.x - enemyUnitPos.x) * (unitPos.x - enemyUnitPos.x) + (unitPos.y - enemyUnitPos.y)*(unitPos.y - enemyUnitPos.y));

				if (distance < 6) // get from unit chase range
				{
					// Set Target
					squadRed->setTarget(redUnit-601, enemyUnitPos.x, enemyUnitPos.y);					
				}
				 if (distance < 3)// get from unit attack range
				{
					// Attack Yellow
					squadYellow->modifyHealth(enemyTargetID-501, -30);;	
				}			 			

				/*std::cout << "target ID" << enemyTargetID << "\n";
				std::cout << "\n Red unit: " << unitPos.x  << " " << unitPos.y;
				std::cout << "\n Yellow unit: " << enemyUnitPos.x << " " << enemyUnitPos.y ;
				std::cout <<"\n dist: "  << distance << "\n";*/
			}
		}
	}

	std::list<int> yelUnitsActive = squadYellow->getActiveUnitsIDs();
	for (int yelUnit : yelUnitsActive)
	{
		if (squadYellow->getUnitTarget(yelUnit - 501) > 0)
		{
			int enemyTargetID = squadYellow->getUnitTarget(yelUnit - 501);

			if (enemyTargetID > 600)
			{
				Vector2 enemyUnitPos = squadRed->getUnitPosition(enemyTargetID - 601);
				Vector2 unitPos = squadYellow->getUnitPosition(yelUnit - 501);

				float distance = sqrt((unitPos.x - enemyUnitPos.x) * (unitPos.x - enemyUnitPos.x) + (unitPos.y - enemyUnitPos.y) * (unitPos.y - enemyUnitPos.y));

				if (distance < 6)
				{
					squadYellow->setTarget(yelUnit - 501, enemyUnitPos.x, enemyUnitPos.y);
				}
				if (distance < 3)
				{
					squadRed->modifyHealth(enemyTargetID - 601, -30);
				}
			}
		}
	}
}//---


//=================================================================================


//=================================================================================



void Game::updateGameObjects()
{
	int targetTileX = -1, targetTileY = -1;


	// When L mouse is clicked
	if (playerInput.mouseL)
	{
		// Move the target Marker
		targetMarker->x = playerInput.mouseX;
		targetMarker->y = playerInput.mouseY;

		// Get Tile Clicked on
		targetTileX = gameAI->findTileXFromMouseX(map, playerInput.mouseX);
		targetTileY = gameAI->findTileYFromMouseY(map, playerInput.mouseY);

		// GetUnitClicked on
		unitSelected = -1; // reset selection
		unitSelected = squadRed->findUnit(targetTileX, targetTileY);
		if (unitSelected < 0) unitSelected = squadYellow->findUnit(targetTileX, targetTileY);
	}

	// When R mouse is clicked
	if (playerInput.mouseR)
	{
		// Move the target Marker
		targetMarker->x = playerInput.mouseX;
		targetMarker->y = playerInput.mouseY;

		// Get Tile Clicked on
		targetTileX = gameAI->findTileXFromMouseX(map, playerInput.mouseX);
		targetTileY = gameAI->findTileYFromMouseY(map, playerInput.mouseY);

		if (unitSelected > 500 && unitSelected < 600) // Yellow
		{
			int unit = unitSelected - 501;
			squadYellow->setTarget(unit, targetTileX, targetTileY);
		}
		else if (unitSelected > 600 && unitSelected < 700) // Red
		{
			int unit = unitSelected - 601;
			squadRed->setTarget(unit, targetTileX, targetTileY);
		}
	}



	//  Update Sqauds  -------- Send each squad map and mouse
	squadYellow->update(map, targetTileX, targetTileY, playerInput.mouseL);
	squadRed->update(map, targetTileX, targetTileY, playerInput.mouseL);



	// ------------------------------------------------------------------------

	// add timer to only call every time period *******

	if (nextSlowUpdateTime < SDL_GetTicks64())
	{
		findNearestTarget();

		nextSlowUpdateTime = SDL_GetTicks64() + 1000;
	}
	//--------------------------------------------------------------------------


	// Modify Display
	adjustMapDisplay();
}//---


//=================================================================================

void Game::adjustMapDisplay()
{
	// key for zoom in (= / +)  out (-)
	if (playerInput.keyPressed == SDLK_EQUALS || playerInput.mouseWheelUp == true)
	{
		map->setMapTileSize(+1);
		bg_map->setMapTileSize(+1);
	}

	if (playerInput.keyPressed == SDLK_MINUS || playerInput.mouseWheelDown == true) {map->setMapTileSize(-1); bg_map->setMapTileSize(-1);
	}

	// Move Board UPLR
	if (playerInput.keyPressed == SDLK_LEFT || playerInput.keyPressed == SDLK_a) { map->moveMapX(1); bg_map->moveMapX(1); }
	if (playerInput.keyPressed == SDLK_RIGHT || playerInput.keyPressed == SDLK_d) { map->moveMapX(-1); bg_map->moveMapX(-1); }
	if (playerInput.keyPressed == SDLK_UP || playerInput.keyPressed == SDLK_w) {map->moveMapY(1); map->moveMapY(1); }
	if (playerInput.keyPressed == SDLK_DOWN || playerInput.keyPressed == SDLK_s) {map->moveMapY(-1); bg_map->moveMapY(-1);}
}//---


//=================================================================================

void Game::render()
{
	SDL_RenderClear(renderer);


	bg_map->drawBGMap();
	// Map Tiles
	map->drawMap();

	// objects
	targetMarker->render();
	overlay->render();

	updateGUI();

	SDL_RenderPresent(renderer);
}//---


//=================================================================================

void Game::handleEvents() // Updates Player Input
{
	// Reset Inputs
	playerInput.keyPressed = NULL;
	playerInput.mouseL = false;
	playerInput.mouseR = false;
	playerInput.mouseWheelUp = false;
	playerInput.mouseWheelDown = false;

	SDL_PollEvent(&playerInputEvent);

	switch (playerInputEvent.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;

	case SDL_MOUSEMOTION:
		int mouseXpos, mouseYpos;
		SDL_GetMouseState(&mouseXpos, &mouseYpos);
		playerInput.mouseX = mouseXpos;
		playerInput.mouseY = mouseYpos;
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (playerInputEvent.button.button == SDL_BUTTON_LEFT) playerInput.mouseL = true;
		if (playerInputEvent.button.button == SDL_BUTTON_RIGHT) playerInput.mouseR = true;
		break;

	case SDL_MOUSEWHEEL:
		if (playerInputEvent.wheel.y > 0) playerInput.mouseWheelUp = true;
		if (playerInputEvent.wheel.y < 0) playerInput.mouseWheelDown = true;
		break;

	case SDL_KEYDOWN:

		playerInput.keyPressed = playerInputEvent.key.keysym.sym;
		break;

	default:
		break;
	}
}//---


 //=================================================================================

void Game::updateGUI()
{
	std::string  screenText;
	int textW = 0, textH = 0;

	// Squad Data to Text
	screenText = "                                                                    ";
	screenText += "Red Sqaud ";
	screenText += " - Res: " + std::to_string(squadRed->getResources());
	screenText += " - Units: " + std::to_string(squadRed->getActiveUnits());

	screenText += "\n                                                                    ";
	screenText += "Yel Sqaud ";
	screenText += " - Res: " + std::to_string(squadYellow->getResources());
	screenText += " - Units: " + std::to_string(squadYellow->getActiveUnits());

	screenText += "\n\n\n ----------";
	screenText += "\n Selected: " + std::to_string(unitSelected);
	screenText += "\n Unit States: ";
	if (unitSelected > 500 && unitSelected < 600)
	{
		screenText += "\n HP: " + std::to_string((int)squadYellow->getUnitHealth(unitSelected - 501));
	}
	if (unitSelected > 600 && unitSelected < 700)
	{
		screenText += "\n HP: " + std::to_string((int)squadRed->getUnitHealth(unitSelected - 601));
		Vector2 unitPos = squadRed->getUnitPosition(unitSelected - 601);
		screenText += "\n Pos: " + std::to_string(unitPos.x) + " " + std::to_string(unitPos.y);
		screenText += "\n targ: " + std::to_string((int)squadRed->getUnitTarget(unitSelected - 601));
	}

	// Create Text Texture
	//textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
	textSurface = TTF_RenderText_Blended_Wrapped(font, screenText.c_str(), textColour, 0);

	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 16, 16, textW, textH };

	// Copy Text Texture to Renderer
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	// Clear the Memory
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

}//---


//=================================================================================

void Game::init(const char* pTitle, int pXpos, int pYpos, int pWidth, int pHeight, bool pFullscreen)
{
	int flags = 0;
	if (pFullscreen) flags = SDL_WINDOW_FULLSCREEN;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "SDL Initialised  \n";
		// Create Game Window
		gameWindow = SDL_CreateWindow(pTitle, pXpos, pYpos, pWidth, pHeight, flags);
		if (gameWindow)	std::cout << "Game Window Created \n";
		// Create Renderer
		renderer = SDL_CreateRenderer(gameWindow, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 180, 180, 180, 0);
			std::cout << "Renderer Created \n";
		}
		// Initialise Fonts
		TTF_Init();
		font = TTF_OpenFont("assets/fonts/arial.ttf", 14);
		if (font) std::cout << "TTF initialise \n";

		// Init AUDIO
		Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
		std::printf("Audio Mixer Initialised \n");

		// Set initial states
		isRunning = true;
	}
	else
	{
		isRunning = false;
	}
}//---

//=================================================================================

void Game::clean()
{
	std::cout << "************************* \nClearing Memory \n";
	SDL_Delay(1000);

	//AUDIO
	Mix_FreeMusic(music);
	Mix_FreeChunk(shootSound);
	Mix_CloseAudio();

	// Fonts
	TTF_CloseFont(font);
	TTF_Quit();


	SDL_DestroyWindow(gameWindow);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "SDL Closed \n";

}//---

//=================================================================================
