#pragma once
#include "Game.h"

//=================================================================================
struct Node
{	
	int fCost = 0, gCost = 0, hCost = 0;
	int xPos = 0, yPos = 0;
	int parentX = 0;
	int parentY = 0;
	bool hasParent = false;

	// Equality Operator
	bool operator == (const Node& node) // RHS 
	{		
		return node.fCost && node.hasParent && node.parentX	&& node.parentY; 		
	}
};


//=================================================================================

class Level
{
public:

	// PathFinding
	std::list<Vector2> findAdjacentTiles(Vector2 currentPos);
	std::list<Vector2> findPath(Vector2 currentPos, Vector2 targetPos);
	int findCost(Vector2 startPos, Vector2 targetPos);

	std::list<Node> openSet = {};
	std::list<Node> closedSet = {};

	bool pathExists = false;



	//-----------------------------------------------------


	Level();  // Constructor 

	void loadMap(int mapArray[BOARD_HEIGHT][BOARD_WIDTH]);
	void drawMap();

	int  getTileContent(int x, int y);
	bool setTile(int x, int y, int tileType);

	void moveMapX(int distance);
	void moveMapY(int distance);

	int getStartX();
	int getStartY();

	void setMapTileSize(int size);
	int getMapTileSize();

	void removeDoor();
	void removeKeys();

	// Waypoints
	void createWaypoints();
	bool checkTargetInLineOfSight(Vector2 pStartPos, Vector2 pTargtetPos);
	Vector2 getNearestWaypointPos(Vector2 position);
	std::list<int> getListOfWaypointsToTarget(Vector2 pStartPos, Vector2 pTargtetPos);
	Vector2 getWPPos(int index) { return wayPointPositions[index]; };
	Vector2 getNextRoomWPPos(Vector2 currentPos);
	Vector2 getNearestKeyPos(Vector2 currentPos);

private:

	// Tile Textures
	Vector2	wayPointPositions[25] = {};

	SDL_Rect src, dest;
	SDL_Texture* empty;
	SDL_Texture* wall;
	SDL_Texture* border;
	SDL_Texture* key;
	SDL_Texture* warrior;
	SDL_Texture* ranger;
	SDL_Texture* mage;
	SDL_Texture* goblin;
	SDL_Texture* skeleton;
	SDL_Texture* demon;
	SDL_Texture* spawn;
	SDL_Texture* ruin_door;
	SDL_Texture* ruin_wall;
	SDL_Texture* ruin_floor;

	// Maps Array and Size
	int map[BOARD_HEIGHT][BOARD_WIDTH];
	int mapStartX = 96;
	int mapStartY = 64;
	int mapTileSize = SPRITE_SIZE;
};
