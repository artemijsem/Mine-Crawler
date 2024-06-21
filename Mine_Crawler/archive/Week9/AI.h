#pragma once
#include "GameObjects.h"
#include "Levels.h"
// This will contain
// Path finding 

//=================================================================================


class AI
{
public:
	

	int findTileXFromMouseX(Level* pMap, int pMouseX);
	int findTileYFromMouseY(Level* pMap, int pMouseY);

	std::list<Vector2>findTilesinSelection(Level* pMap, Vector2 startPos, Vector2 endPos);



	std::list<Vector2> findAdjacentRandomTile(Level* pMap, int x, int y);

	// find  nearest object by type


	

private:

};

//=================================================================================
