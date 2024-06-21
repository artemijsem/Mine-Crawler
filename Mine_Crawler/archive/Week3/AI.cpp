#include "AI.h"


//=================================================================================

void AI::getNextTile()
{

}


//=================================================================================


int AI::findTileX(Level* pMap, int pMouseX)
{
	int targetTileX = (pMouseX - pMap->getStartX()) / pMap->GetMapTileSize();

	return targetTileX;
}

int AI::findTileY(Level* pMap, int pMouseY)
{
	int targetTileY = (pMouseY - pMap->getStartY()) / pMap->GetMapTileSize();

	return targetTileY;
}


//=================================================================================

