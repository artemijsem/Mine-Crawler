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

// Create  Squads
Squad* squadYellow;
Squad* squadRed;

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

	// Create Teams (Max units = 50)
	// name, tileType, start x y 
	squadRed = new Squad("Red Team", 6, 30, 5);
	squadRed->createUnits(map, 2, 1); // initial spawn amount and initial state

	squadYellow = new Squad("Yellow Team", 5, 10, 10);
	squadYellow->createUnits(map, 2, 3);

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
	int targetTileX = -1, targetTileY = -1;

	// Move the target Marker
	if (playerInput.mouseL)
	{
		targetMarker->x = playerInput.mouseX;
		targetMarker->y = playerInput.mouseY;

		// Get Tile Clicked on
		targetTileX = gameAI->findTileXFromMouseX(map, playerInput.mouseX);
		targetTileY = gameAI->findTileYFromMouseY(map, playerInput.mouseY);
	}	
	
	//  Update Sqauds  -------- Send each squad map and mouse
	squadYellow->update(map, targetTileX, targetTileY, playerInput.mouseL);
	squadRed->update(map, targetTileX, targetTileY, playerInput.mouseL);


	// Modify Display
	adjustMapDisplay();
}//---


//=================================================================================

void Game::adjustMapDisplay()
{
	// key for zoom in (= / +)  out (-)
	if (playerInput.keyPressed == SDLK_EQUALS || playerInput.mouseWheelUp == true) map->setMapTileSize(+1);

	if (playerInput.keyPressed == SDLK_MINUS || playerInput.mouseWheelDown == true) map->setMapTileSize(-1);

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

	// SuqaD Data to Text
	screenText = "Team: Red Sqaud ";
	screenText += " - Res: " + std::to_string(squadRed->getResources());
	screenText += " - Units: " + std::to_string(squadRed->getActiveUnits());

	screenText += "\nTeam:  Yel Sqaud ";
	screenText += " - Res: " + std::to_string(squadYellow->getResources());
	screenText += " - Units: " + std::to_string(squadYellow->getActiveUnits());

	// Create Text Texture
	//textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
	textSurface = TTF_RenderText_Blended_Wrapped(font, screenText.c_str(), textColour, 0);

	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 8, 8, textW, textH };

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
