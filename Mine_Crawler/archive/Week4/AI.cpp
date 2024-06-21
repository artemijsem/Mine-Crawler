#include "AI.h"


//=================================================================================

void AI::getNextTile()
{

}


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


//=================================================================================

