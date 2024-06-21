#include "AI.h"



//=================================================================================


int AI::findTileXFromMouseX(Level* pMap, int pMouseX)
{
	int targetTileX = (pMouseX - pMap->getStartX()) / pMap->getMapTileSize();

	return targetTileX;
}

int AI::findTileYFromMouseY(Level* pMap, int pMouseY)
{
	int targetTileY = (pMouseY - pMap->getStartY()) / pMap->getMapTileSize();

	return targetTileY;


}

std::list<Vector2> AI::findAdjacentRandomTile(Level* pMap, int pX, int pY)
{	
	std::list<Vector2> path = {};
	path.pop_front();

	std::cout << path.size();

	return path;
}

//=================================================================================

