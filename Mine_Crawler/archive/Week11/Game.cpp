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
Level* map;
AI* gameAI;

// Create Squads
Squad* squadYellow;
Squad* squadRed;

// Game objects
Item* targetMarker;
Item* overlay;
Item* highLights[50];

Attack* bulletsRed[100]; // New
Item* hits[100];

//================================================================================
// Variables
//int unitSelected = -1;



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
	// Create Game Level Map
	map = new Level();


	// Create Teams (Max units = 50)
	// Yellow Squad NPCs
	squadYellow = new Squad("Yellow Team", 5, 50, 25); // name, tileType, start x y 
	squadYellow->createUnits(map, 2, 2); // level, spawn amount & state

	// Red Squad - Player PCs
	squadRed = new Squad("Red Team", 6, 10, 5); // name, tileType, start x y 
	squadRed->createUnits(map, 2, 0);

	// Create other Objects ------------------------------------------------

	// show mouse click pos with target Marker
	targetMarker = new Item("assets/images/Square_Purple.png", 16, 16, 8);

	// Selection highlights
	for (int i = 0; i < sizeof(highLights) / sizeof(highLights[0]); i++)
	{
		highLights[i] = new Item("assets/images/square_cross_yellow.png", -1, -1, 12);
	}

	// Bullets
	for (int i = 0; i < sizeof(bulletsRed) / sizeof(bulletsRed[0]); i++)
	{
		bulletsRed[i] = new Attack("assets/images/tiles/shot.png", 300, 300, 16);
	}

	// Sprite for Interface Overlay
	overlay = new Item("assets/images/Interface.png", 16, 16, 800);
	overlay->srcRect.w = 800; overlay->srcRect.h = 600;
	overlay->destRect.w = 800; overlay->destRect.h = 600;
}//---

//=================================================================================

void Game::welcomeScreen()
{

}//---

void Game::playAgainScreen()
{

}//---



//=================================================================================
void Game::findNearestTarget() // Only for Red Units 
{
	// Get a list of the active units
	std::list<int> yelActiveUnits = squadYellow->getActiveUnitsIDs();
	std::list<int> redUnitsActive = squadRed->getActiveUnitsIDs();

	// Loop through all active Red units to find nearest enemy unit
	for (int redUnit : redUnitsActive)
	{
		// Reset distance variables
		int nearestTarget = 0;
		float distanceToNearestUnit = 1000; //  a large number off the board initially

		// get the current active Red unit's position
		Vector2 unitPos = squadRed->getUnitPosition(redUnit - 601);

		// loop through active yellow units 
		for (int yelUnit : yelActiveUnits)
		{
			// get loop unit Position
			Vector2 enemyUnitPos = squadYellow->getUnitPosition(yelUnit - 501);

			// Calc distance using pythagoras
			float distance = sqrt((unitPos.x - enemyUnitPos.x) * (unitPos.x - enemyUnitPos.x) + (unitPos.y - enemyUnitPos.y) * (unitPos.y - enemyUnitPos.y));

			if (distance < distanceToNearestUnit) // check if this unit is closer
			{
				distanceToNearestUnit = distance;
				nearestTarget = yelUnit;
			}
		}
		// Set Target of current red unit		
		squadRed->setUnitTarget(map, redUnit - 601, nearestTarget);
		squadRed->setUnitTargetDistance(redUnit - 601, distanceToNearestUnit);
	}

}//---


//=================================================================================


void attackTarget() // Do red units have an attack Target in Range
{
	// Loop through all active Red units
	std::list<int> redUnitsActive = squadRed->getActiveUnitsIDs();

	for (int redUnit : redUnitsActive)
	{
		if (squadRed->getUnitTarget(redUnit - 601) > 0)
		{
			// get the target enemy ID 
			int enemyTargetID = squadRed->getUnitTarget(redUnit - 601);

			if (enemyTargetID > 500 && enemyTargetID < 600) //-- Yellow
			{
				Vector2 enemyUnitPos = squadYellow->getUnitPosition(enemyTargetID - 501);
				Vector2 unitPos = squadRed->getUnitPosition(redUnit - 601);

				// Calc distance using pythagoras
				float distance = sqrt((unitPos.x - enemyUnitPos.x) * (unitPos.x - enemyUnitPos.x) + (unitPos.y - enemyUnitPos.y) * (unitPos.y - enemyUnitPos.y));

				if (distance < squadRed->getUnitChaseRange(redUnit - 601) && distance > squadRed->getUnitAttackRange(redUnit - 601))
				{
					// Set Target
					squadRed->setTarget(map, redUnit - 601, enemyUnitPos.x, enemyUnitPos.y);
					squadRed->setUnitState(redUnit - 601, 3);

				}
				if (distance < squadRed->getUnitAttackRange(redUnit - 601))
				{
					float angle = std::atan2(unitPos.x - enemyUnitPos.x, unitPos.y - enemyUnitPos.y) * -180 / M_PI;

					// find first inactive bullet
					for (int i = 0; i < sizeof(bulletsRed) / sizeof(bulletsRed[0]); i++)
					{
						if (!bulletsRed[i]->getIsActive())
						{
							int spawnX = unitPos.x * map->getMapTileSize() + map->getStartX(); // convert to screen
							int spawnY = unitPos.y * map->getMapTileSize() + map->getStartY();

							bulletsRed[i]->spawn(spawnX, spawnY, angle, map);
							break;
						}
					}
				}
			}
		}
	}
}//-----

//=================================================================================

Vector2 startSelectPos;
Vector2 endSelectPos;
std::list<int> unitsSelected = {};

void Game::selectUnits() // GROUP SELECT base on box drawn by mouse
{
	if (playerInput.mouseL) //mouse down
	{
		// reset end Pos and get start pos - box top left
		endSelectPos.x = startSelectPos.x = playerInput.mouseX;
		endSelectPos.y = startSelectPos.y = playerInput.mouseY;
	}


	if (playerInput.mouseUpL) // mouse released
	{
		endSelectPos.x = playerInput.mouseX;
		endSelectPos.y = playerInput.mouseY; // box end

		// find list of tiles in box from AI Class		
		std::list<Vector2> tilesFound = gameAI->findTilesinSelection(map, startSelectPos, endSelectPos);


		unitsSelected.clear(); // clear list of units

		for (Vector2 tile : tilesFound) // search for units in tiles
		{
			int unitFound = -1;
			unitFound = squadRed->findUnit(tile.x, tile.y);
			if (unitFound > 0) unitsSelected.push_back(unitFound); // add unit to list
		}
	}

	//  ************ ASIGN TARGET **************** Right click
	// Reset Target
	int targetTileX = -1, targetTileY = -1;

	// When R mouse is clicked  
	if (playerInput.mouseR)
	{
		// Move the target Marker
		targetMarker->x = playerInput.mouseX;
		targetMarker->y = playerInput.mouseY;

		// Get Tile Clicked on
		targetTileX = gameAI->findTileXFromMouseX(map, playerInput.mouseX);
		targetTileY = gameAI->findTileYFromMouseY(map, playerInput.mouseY);

		if (unitsSelected.size() > 0)
		{
			for (int unit : unitsSelected)
			{
				if (unit > 600 && unit < 700) // only for red units
				{
					int unitSelected = unit - 601; // subtract 601 to get real pos in array						
					squadRed->setTarget(map, unitSelected, targetTileX, targetTileY);
				}
			}
		}
	}
}//---


//=================================================================================

void Game::highlightSelection()
{
	// Reset all highlights
	for (int i = 0; i < sizeof(highLights) / sizeof(highLights[0]); i++)
	{
		highLights[i]->isActive = false;
	}

	if (unitsSelected.size() > 0) // only highight if selction active
	{
		int tileSize = map->getMapTileSize();
		for (int unit : unitsSelected)
		{
			if (unit > 600 && unit < 700) // only for red units
			{
				int unitSelected = unit - 601;
				Vector2 unitPos = squadRed->getUnitPosition(unitSelected); // current tile pos	

				// draw highligh Rect instead of sprite

				// find first unused highlight
				for (int i = 0; i < sizeof(highLights) / sizeof(highLights[0]); i++)
				{
					if (!highLights[i]->getIsActive())
					{
						highLights[i]->isActive = true;
						highLights[i]->x = unitPos.x * tileSize + map->getStartX() + tileSize / 4;
						highLights[i]->y = unitPos.y * tileSize + map->getStartY();
						break;
					}
				}
			}
		}
	}

}//---

//=================================================================================

void Game::manageGroups()
{
	// assign group
	if (playerInput.keyPressed == 49) // 1 Pressed
	{
		// clear all unit group settings ****************

		std::cout << "\nsetting group 1 ";

		if (unitsSelected.size() > 0)
		{
			squadRed->assignGroup1(unitsSelected);

			// Loop through all members of group
			for (int unit : unitsSelected)
			{
				//std::cout << " " << unit;
				int unitInSelection = unit - 601;
				squadRed->setUnitGroup(unitInSelection, 1); // set to group 1			
			}

			// set Group Leaeder
			int firstUnit = unitsSelected.front();
			//std::cout << " leader is" << firstUnit;
			firstUnit -= 601;
			squadRed->setGroupLeader(firstUnit, true);
		}
	}

	// get group
	if (playerInput.keyPressed == 54)
	{
		unitsSelected = squadRed->getGroup1();
	}

	// set formation		

	// set goal // explore // gather //

}//---

//=================================================================================

void Game::manageAttacks()
{
	// Loop through Red Bullets Check Collison and Update 
	for (int i = 0; i < sizeof(bulletsRed) / sizeof(bulletsRed[0]); i++)
	{
		if (bulletsRed[i]->getIsActive())
		{
			int tileCollidingX = gameAI->findTileXFromMouseX(map, bulletsRed[i]->x);
			int tileCollidingY = gameAI->findTileYFromMouseY(map, bulletsRed[i]->y);

			// check if an object is in the Tile the bullet is currently over
			int objectColldingWith = map->getTile(tileCollidingX, tileCollidingY);

			if (objectColldingWith > 0) // bullet hit something
			{

				if (objectColldingWith == 1) // Bullet hit Wall
				{
					bulletsRed[i]->isActive = false; // Destroy bullet
				}


				if (objectColldingWith == 5) // Yellow Unit Hit
				{
					// Destroy bullet
					bulletsRed[i]->isActive = false;

					// Get a list of the active Yellow units
					std::list<int> yelActiveUnits = squadYellow->getActiveUnitsIDs();

					// loop through active yellow units 
					for (int unit : yelActiveUnits)
					{
						// get loop unit Position
						Vector2 yellowUnitPos = squadYellow->getUnitPosition(unit - 501);

						// check if is colliding with bullet
						if (yellowUnitPos.x == tileCollidingX && yellowUnitPos.y == tileCollidingY)
						{
							// find Yellow Tile and apply damage									
							squadYellow->modifyHealth(unit - 501, -bulletsRed[i]->getDamage());
						}
					}
				}

				// Something Else Hit

			}

			// update Bullet Movement and life
			bulletsRed[i]->update(map);
		}
	}
}//---


//=================================================================================

void Game::moveGameObjects(float  pMoveAmountX, float pMoveAmountY)
{ // Move Objectct as the map moves

	// Bullets
	for (int i = 0; i < sizeof(bulletsRed) / sizeof(bulletsRed[0]); i++)
	{
		if (bulletsRed[i]->getIsActive())
		{
			bulletsRed[i]->x += pMoveAmountX;
			bulletsRed[i]->y += pMoveAmountY;
		}
	}
}//---

//=================================================================================



void Game::updateGameObjects()
{
	// Red Units Behaviours
	selectUnits();
	highlightSelection();

	// Groups
//	manageGroups();
	manageAttacks();

	//--------------------------------------------------------------------------
	// add timer to only call every time period *******
	if (nextSlowUpdateTime < SDL_GetTicks64())
	{
		findNearestTarget();
		attackTarget();
		nextSlowUpdateTime = SDL_GetTicks64() + 1000;
	}

	//--------------------------------------------------------------------------
	//  Update Sqauds 
	squadYellow->update(map);
	squadRed->update(map);

	// Modify Display
	adjustMapDisplay();
}//---


//=================================================================================


void Game::clearAttacks()
{
	// Bullets
	for (int i = 0; i < sizeof(bulletsRed) / sizeof(bulletsRed[0]); i++)
	{
		bulletsRed[i]->isActive = false;
	}
}//----


void Game::adjustMapDisplay()
{
	// Move Board UPLR
	Vector2 moveDirection = { 0,0 };

	// keys for zoom in (= / +)  out (-)
	if (playerInput.keyPressed == SDLK_EQUALS || playerInput.mouseWheelUp == true)
	{
		map->setMapTileSize(+1);
		moveDirection.x--;
		moveDirection.y--;
		clearAttacks();
	}
	if (playerInput.keyPressed == SDLK_MINUS || playerInput.mouseWheelDown == true)
	{
		map->setMapTileSize(-1);
		moveDirection.x++;
		moveDirection.y++;
		clearAttacks();
	}

	// Check Keys for moving map
	if (playerInput.keyPressed == SDLK_LEFT || playerInput.keyPressed == SDLK_a) moveDirection.x++;
	if (playerInput.keyPressed == SDLK_RIGHT || playerInput.keyPressed == SDLK_d) moveDirection.x--;
	if (playerInput.keyPressed == SDLK_UP || playerInput.keyPressed == SDLK_w) moveDirection.y++;
	if (playerInput.keyPressed == SDLK_DOWN || playerInput.keyPressed == SDLK_s) moveDirection.y--;


	// Check mouse Pos 
	if (SDL_GetTicks64() > nextSlowMouseUpdateTime)
	{
		if (playerInput.mouseX > 780 && playerInput.mouseX < 799) moveDirection.x--;
		if (playerInput.mouseX > 0 && playerInput.mouseX < 20) moveDirection.x++;
		if (playerInput.mouseY > 580 && playerInput.mouseY < 599) moveDirection.y--;
		if (playerInput.mouseY > 0 && playerInput.mouseY < 20) moveDirection.y++;

		nextSlowMouseUpdateTime = SDL_GetTicks64() + 100; // limit speed of mouse moving map
	}

	// Move the Map
	if (moveDirection.x != 0 || moveDirection.y != 0)
	{
		map->moveMapX(moveDirection.x);
		map->moveMapY(moveDirection.y);
		// Move game objects as we move the map
		moveGameObjects(moveDirection.x * map->getMapTileSize(), moveDirection.y * map->getMapTileSize());
	}
}//---


//=================================================================================

void Game::render()
{
	SDL_RenderClear(renderer);

	// Map Tiles
	map->drawMap();

	// Highlights
	for (int i = 0; i < sizeof(highLights) / sizeof(highLights[0]); i++)
	{
		if (highLights[i]->getIsActive()) highLights[i]->render();
	}

	// Bullets
	for (int i = 0; i < sizeof(bulletsRed) / sizeof(bulletsRed[0]); i++)
	{
		if (bulletsRed[i]->getIsActive())
		{
			bulletsRed[i]->render();
		}
	}


	// HP Bars
	squadRed->drawUnitHP(map);
	squadYellow->drawUnitHP(map);

	// objects
	targetMarker->render();
	overlay->render();

	// Screen Text
	updateGUI();

	SDL_SetRenderDrawColor(Game::renderer, 50, 120, 50, 255);
	SDL_RenderPresent(renderer);
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
	screenText += "\n map stX: " + std::to_string(map->getStartX());
	screenText += "\n map stY: " + std::to_string(map->getStartY());

	//if (unitSelected > 500 && unitSelected < 600)
	//{
	//	screenText += "\n HP: " + std::to_string((int)squadYellow->getUnitHealth(unitSelected - 501));
	//}
	//if (unitSelected > 600 && unitSelected < 700)
	//{
	//	screenText += "\n HP: " + std::to_string((int)squadRed->getUnitHealth(unitSelected - 601));
	//	Vector2 unitPos = squadRed->getUnitPosition(unitSelected - 601);
	//	screenText += "\n Pos: " + std::to_string(unitPos.x) + " " + std::to_string(unitPos.y);
	//	screenText += "\n targ: " + std::to_string((int)squadRed->getUnitTarget(unitSelected - 601));
	//}

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


void Game::handleEvents() // Updates Player Input
{
	// Reset Inputs
	playerInput.keyPressed = NULL;
	playerInput.mouseL = false;
	playerInput.mouseR = false;
	playerInput.mouseUpL = false;
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

	case SDL_MOUSEBUTTONUP:

		if (playerInputEvent.button.button == SDL_BUTTON_LEFT)
			playerInput.mouseUpL = true;
		break;

	case SDL_MOUSEWHEEL:
		if (playerInputEvent.wheel.y > 0) playerInput.mouseWheelUp = true;
		if (playerInputEvent.wheel.y < 0) playerInput.mouseWheelDown = true;
		break;

	case SDL_KEYDOWN:

		playerInput.keyPressed = playerInputEvent.key.keysym.sym;
		//std::cout << playerInputEvent.key.keysym.sym;
		break;

	default:
		break;
	}
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
			SDL_SetRenderDrawColor(renderer, 128, 128, 128, 0);
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
