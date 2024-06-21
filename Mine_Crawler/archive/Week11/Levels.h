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

	void setMapTileSize(int size);
	int getMapTileSize();

	// Waypoints
	void createWaypoints();
	bool checkTargetInLineOfSight(Vector2 pStartPos, Vector2 pTargtetPos);
	Vector2 getNearestWaypointPos(Vector2 position);
	std::list<int> getListOfWaypointsToTarget(Vector2 pStartPos, Vector2 pTargtetPos);

	Vector2 getWPPos(int index) { return wayPointPositions[index]; };


private:

	// Tile Textures
	Vector2	wayPointPositions[23] = {};

	SDL_Rect src, dest;
	SDL_Texture* empty;
	SDL_Texture* wall;
	SDL_Texture* border;
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
