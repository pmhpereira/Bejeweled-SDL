#pragma once

#include <SDL.h>

#include "Pair.hpp"
#include "Animation.hpp"

enum GemAnimation
{
	None,
	Switch,
	Destroy,
	Fall
};

class Gem
{
private:
	int _x, _y;
	int _id;

	bool _initialized;

	SDL_Rect _rect;

	SDL_Texture* _texture;

	Animation _animation;

public:
	Gem();
	Gem(int, SDL_Renderer*, SDL_Surface*);

	int GetID();

	void SetPosition(int, int);
	Pair GetBoardPosition();

	void Draw(SDL_Renderer*, SDL_Rect*);

	SDL_Rect BuildRectFromParent(SDL_Rect*);
	bool IsMouseInside(SDL_Rect*, int, int);

	void SetAnimation(Animation);
	Animation GetAnimation();
};
