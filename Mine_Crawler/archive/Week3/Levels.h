#pragma once
#include "Game.h"

class Level
{
public:
	Level();  // Constructor 

	void loadMap(int mapArray[BOARD_HEIGHT][BOARD_WIDTH]);
	void drawMap();

	int  getTile(int x, int y);
	bool setTile(int x, int y, int tileType);
	
	void moveMapX(int distance);
	void moveMapY(int distance);

	int getStartX();
	int getStartY();

	void SetMapTileSize(int size);
	int GetMapTileSize();


private:

	// Tile Textures
	
	SDL_Rect src, dest;
	SDL_Texture* empty;
	SDL_Texture* wall;
	SDL_Texture* water;
	SDL_Texture* resource;
	SDL_Texture* item;
	SDL_Texture* char1;
	SDL_Texture* char2;

	// Maps Array and Size
	int map[BOARD_HEIGHT][BOARD_WIDTH];
	int mapStartX = 96;
	int mapStartY = 64;
	int mapTileSize = SPRITE_SIZE;
};
