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

std::list<Vector2> AI::findTilesinSelection(Level* pMap, Vector2 startPos, Vector2 endPos)
{
	std::list<Vector2> tilesFound = {};

	int mapStartX = pMap->getStartX();
	int mapStartY = pMap->getStartY();
	int mapTileSize = pMap->getMapTileSize();

	int rowsInSelection = 0;
	int colsInSelection = 0;

	colsInSelection = abs((startPos.x - endPos.x) / mapTileSize);
	rowsInSelection = abs((startPos.y - endPos.y) / mapTileSize);

	Vector2 startTile;
	startTile.x = (startPos.x - mapStartX) / mapTileSize;
	startTile.y = (startPos.y - mapStartY) / mapTileSize;

	tilesFound.push_back(startTile);

	if (colsInSelection > 0 || rowsInSelection > 0) //  more than one tile selected
	{
		Vector2 newTile;

		for (int row = 0; row < rowsInSelection; row++)
		{
			for (int col = 0; col < colsInSelection; col++)
			{
				newTile.x = startTile.x + col;
				newTile.y = startTile.y + row;
				tilesFound.push_back(newTile);
			}
		}
	}
	return tilesFound;
}//-----


