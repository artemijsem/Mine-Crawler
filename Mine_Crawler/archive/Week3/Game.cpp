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
SDL_Color textColour = { 255, 255, 0 };
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

// Create array of chars
Character* squadYellow[8];
Character* squadRed[8];

Item* targetMarker;

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

	// Create Arrays of Characters  -- TEAM YELLOW -------------------------
	for (int i = 0; i < sizeof(squadYellow) / sizeof(squadYellow[0]); i++)
	{
		// Create Instance & starting positions and spacing
		int xPos = 10 + i;
		int yPos = 10 + i;
		squadYellow[i] = new Character(xPos, yPos);

		// Set initial values
		squadYellow[i]->setType(5); // map tile type 
		squadYellow[i]->setSpeed(1.5f);
		squadYellow[i]->setState(2);
		squadYellow[i]->setHeading(rand() % 8); // random initial Direction

		// is the spawn postion empty
		if (map->getTile(xPos, yPos) == 0)
		{
			map->setTile(xPos, yPos, squadYellow[i]->getType());
			squadYellow[i]->isActive = true;
		}
		else
		{
			std::cout << "Tile occupied, Character: " << i << " not created" << std::endl;
			squadYellow[i]->isActive = false;
		}
	}


	// Create Arrays of Characters  -- TEAM RED    ------------------------

	for (int i = 0; i < sizeof(squadRed) / sizeof(squadRed[0]); i++)
	{		
		// starting positions and instance
		int xPos = 40 + i;
		int yPos = 10 + i;
		squadRed[i] = new Character(xPos, yPos);

		// Set Tile Type to Add to the Map -- Red marker
		squadRed[i]->setType(6);
		squadRed[i]->setSpeed(3);
		squadRed[i]->setState(3);
		squadRed[i]->setHeading(rand() % 8); // random initial Direction

		// is the spawn postion empty
		if (map->getTile(xPos, yPos) == 0)
		{
			map->setTile(xPos, yPos, squadRed[i]->getType());
			squadRed[i]->isActive = true;
		}
		else
		{
			std::cout << "Tile occupied, Character: " << i << " not created" << std::endl;
			squadRed[i]->isActive = false;
		}
	}


	// Create other Object ------------------------------------------------

	// show mouse click pos with target Marker
	targetMarker = new Item("assets/images/Square_Purple.png", 16, 16, 8);	
}//---

//=================================================================================

void Game::welcomeScreen()
{

}//---

void Game::playAgainScreen()
{

}//---


//=================================================================================

void Game::updateGameObjects()
{	
	//  Manage Yellow Squad  --------

	for (int i = 0; i < sizeof(squadYellow) / sizeof(squadYellow[0]); i++)
	{
		squadYellow[i]->update(map);
	}

	//  Manage red Squad --------

	// Set Target 
	if (playerInput.mouseL)
	{	// Move the target Marker
		targetMarker->x = playerInput.mouseX;
		targetMarker->y = playerInput.mouseY;

		// Set the Target for the Squad Members
		for (int i = 0; i < sizeof(squadRed) / sizeof(squadRed[0]); i++)
		{	// Use AI Class to get tile from cursor Pos
			int targetTileX = gameAI->findTileX(map, playerInput.mouseX);
			int targetTileY = gameAI->findTileY(map, playerInput.mouseY);		
			// Check the tile is valid - On the Board 
			if (targetTileX > 0 && targetTileX < BOARD_WIDTH 
				&& targetTileY > 0 && targetTileY < BOARD_HEIGHT)
			{	
				squadRed[i]->setTarget(targetTileX, targetTileY);

				// only Change if idle
				if (squadRed[i]->getState() == 0) // idle
				{
					squadRed[i]->setState(3); /*set idle units to chase*/
					squadRed[i]->setTarget(targetTileX, targetTileY);
				}
			}
		}
	}

	for (int i = 0; i < sizeof(squadRed) / sizeof(squadRed[0]); i++) 
	{
		squadRed[i]->update(map);
	}

	// Modify Display
	adjustMapDisplay();
}//---


//=================================================================================

void Game::adjustMapDisplay()
{
	// key for zoom in (= / +)  out (-)
	if (playerInput.keyPressed == SDLK_EQUALS || playerInput.mouseWheelUp == true) map->SetMapTileSize(+1);

	if (playerInput.keyPressed == SDLK_MINUS || playerInput.mouseWheelDown == true) map->SetMapTileSize(-1);

	// Move Board UPLR
	if (playerInput.keyPressed == SDLK_LEFT || playerInput.keyPressed == SDLK_a) map->moveMapX(1);
	if (playerInput.keyPressed == SDLK_RIGHT || playerInput.keyPressed == SDLK_d) map->moveMapX(-1);
	if (playerInput.keyPressed == SDLK_UP || playerInput.keyPressed == SDLK_w) map->moveMapY(1);
	if (playerInput.keyPressed == SDLK_DOWN || playerInput.keyPressed == SDLK_s) map->moveMapY(-1);
}//---


//=================================================================================

void Game::render()
{
	SDL_RenderClear(renderer);

	// Map Tiles
	map->drawMap();

	// objects
	targetMarker->render();

	updateGUI();

	SDL_RenderPresent(renderer);
}//---


//=================================================================================

void Game::handleEvents() // Updates Player Input
{
	// Reset Inputs
	playerInput.keyPressed = NULL;
	playerInput.mouseL = false;
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
		playerInput.mouseL = true;
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

	// Mouse Data to Text
	screenText = " Mouse: " + std::to_string(playerInput.mouseL);
	screenText += " x: " + std::to_string(playerInput.mouseX);
	screenText += " y: " + std::to_string(playerInput.mouseY);
	screenText += "\n-----------\n Map Start: " + std::to_string(map->getStartX()) + " " + std::to_string(map->getStartY());

	// Create Text Texture
	//textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
	textSurface = TTF_RenderText_Blended_Wrapped(font, screenText.c_str(), textColour, 0);

	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 648, 8, textW, textH };

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
