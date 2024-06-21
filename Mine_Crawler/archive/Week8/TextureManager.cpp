#include "TextureManager.h"

//=================================================================================

SDL_Texture* TextureManager::LoadTexture(const char* pFilename)
{
	SDL_Surface* tempSurface = IMG_Load(pFilename);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
	if (texture == NULL) printf(" Image Load Error  - SDL Error: %s\n", SDL_GetError());

	SDL_FreeSurface(tempSurface);
	return texture;
}//---

//=================================================================================


void TextureManager::Draw(SDL_Texture* pTexture, SDL_Rect pSourceRect, SDL_Rect pDestinationRect)
{
	SDL_RenderCopy(Game::renderer, pTexture, &pSourceRect, &pDestinationRect);

}//---
//=================================================================================
