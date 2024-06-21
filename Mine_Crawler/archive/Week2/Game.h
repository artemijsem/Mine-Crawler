#pragma once

#include <SDL.h>
#include<SDL_Image.h>
#include<SDL_ttf.h>
#include <SDL_mixer.h>
#include<iostream>

// =======================================================
// Constants
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int BOARD_WIDTH = 60;
constexpr int BOARD_HEIGHT = 50;
constexpr int SPRITE_TEX_SIZE = 32; // to Load from file
constexpr int SPRITE_SIZE = 16; // default
constexpr int FPS = 50;

 
 

// =======================================================
struct PlayerInput
{
	int mouseX = 0, mouseY = 0;
	bool mouseL = false, mouseR = false;
	bool mouseWheelUp = false, mouseWheelDown = false;
	int keyPressed = NULL;
}; 

// =======================================================
class Game
{
public:
	//int mainGameCounter = 0;
	static SDL_Renderer* renderer;
	static SDL_Event playerInputEvent;
	bool running() { return isRunning; } // can be read not set

	Game(); // constructor
	void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	void createGameObjects();
	void handleEvents();
	void updateGameObjects();
	void updateGUI();
	void render();

	//--------------
	void adjustMapDisplay();


	//--------------
	void playAgainScreen();
	void welcomeScreen();

	void clean();


private:
	bool isRunning = false;
	SDL_Window* gameWindow = nullptr;

}; 
//============
