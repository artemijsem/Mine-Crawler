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
// Create array of chars
Character* chars[2];
Character* new_chars[5];

Item* targetMarker;
int mapTileSize = SPRITE_SIZE;

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

	// Create Game Objects
	map = new Level();

	// Create Arrays of Characters
	for (int i = 0; i < sizeof(chars) / sizeof(chars[0]); i++)
	{
		// starting positions and spacing
		int xPos = 6 + 2 * i;
		int yPos = 7 + i;

		chars[i] = new Character(xPos, yPos);
		if (map->setTile(xPos, yPos, 5))
		{
			chars[i]->isActive = true;
		}
		else
		{

			std::cout << "Tile occupied, Character: " << i << " not created" << std::endl;
			chars[i]->isActive = false;

			// Create a function that checks adjacent tiles for empty ones
		}
	}

	// Create Arrays of New Characters

	for (int i = 0; i < sizeof(new_chars) / sizeof(new_chars[0]); i++)
	{
		int xPos = 6 + 1 * i;
		int yPos = 30 + i;

		new_chars[i] = new Character(xPos, yPos);
		if (map->setTile(xPos, yPos, 6))
		{
			new_chars[i]->isActive = true;
		}
		else
		{
			std::cout << "Tile occupied, New Character: " << i << " not created" << std::endl;
			new_chars[i]->isActive = false;
		}
	}

	// show mouse click pos
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
	// Move Target Marker
	if (playerInput.mouseL)
	{
		targetMarker->x = playerInput.mouseX;
		targetMarker->y = playerInput.mouseY;


	}

	// Update Characters when functions added
	// Loop through array	

	for (int i = 0; i < sizeof(chars) / sizeof(chars[0]); i++)
	{
		chars[i]->setTarget(targetMarker->x, targetMarker->y);
		chars[i]->update();
		
	}

	adjustMapDisplay();

}//---


//=================================================================================

void Game::adjustMapDisplay()
{
	// key for zoom in (= / +)  out (-)
	if (playerInput.keyPressed == SDLK_EQUALS || playerInput.mouseWheelUp == true)
		if (mapTileSize < 18) mapTileSize++;

	if (playerInput.keyPressed == SDLK_MINUS || playerInput.mouseWheelDown == true)
		if (mapTileSize > 10) mapTileSize--;

	// Move Board by changeing draw start position
	if (playerInput.keyPressed == SDLK_LEFT) map->moveMapX(mapTileSize);
	if (playerInput.keyPressed == SDLK_RIGHT) map->moveMapX(-mapTileSize);
	if (playerInput.keyPressed == SDLK_UP) map->moveMapY(mapTileSize);
	if (playerInput.keyPressed == SDLK_DOWN) map->moveMapY(-mapTileSize);
}//---


//=================================================================================

void Game::render()
{
	SDL_RenderClear(renderer);

	// Map Tiles
	map->drawMap(mapTileSize);

	// objects
	targetMarker->render();

	// Characters
	

	updateGUI();

	SDL_RenderPresent(renderer);
}//---


//=================================================================================

void Game::handleEvents() // Add Mouse Movement
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

	// Create Text Texture
	textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 600, 8, textW, textH };

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
			SDL_SetRenderDrawColor(renderer, 64, 64, 64, 0);
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
