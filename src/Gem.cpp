#include "Gem.hpp"
#include "Board.hpp"

#include "Animation\GemDestroyAnimation.hpp"
#include "Animation\GemFallAnimation.hpp"
#include "Animation\GemSwitchAnimation.hpp"

Gem::Gem()
{
	_id = -1;
	_texture = NULL;

	_rect.w = Board::GEM_SIZE;
	_rect.h = Board::GEM_SIZE;
}

Gem::Gem(int id, SDL_Renderer* renderer, SDL_Surface *surface)
{
	_id = id;
	_texture = SDL_CreateTextureFromSurface(renderer, surface);

	_rect.w = Board::GEM_SIZE;
	_rect.h = Board::GEM_SIZE;
}

int Gem::GetID()
{
	return _id;
}

void Gem::SetPosition(int x, int y)
{
	_x = x;
	_y = y;
}

Pair Gem::GetBoardPosition()
{
	return Pair(_x, _y);
}

void Gem::Draw(SDL_Renderer *renderer, SDL_Rect *parent)
{
	if (_id == -1)
		return;

	if (!_animation.HasEnded())
	{
		_animation.Update();

		SDL_Rect originalRect = BuildRectFromParent(parent);
		SDL_Rect rect = *parent;

		Pair position = _animation.GetPosition();
		rect.x = position.x;
		rect.y = position.y;

		float scale = _animation.GetScale();
		rect.w = (int) (_rect.w * scale);
		rect.h = (int) (_rect.h * scale);
		// make sure the gem stays centered when rescaled
		rect.x += (_rect.w - rect.w) / 2;
		rect.y += (_rect.h - rect.h) / 2;

		float alpha = _animation.GetAlpha();
		SDL_SetTextureAlphaMod(_texture, (int) (alpha * 255));

		SDL_RenderCopy(renderer, _texture, NULL, &rect);
	}
	else
	{
		SDL_Rect rect = BuildRectFromParent(parent);
		SDL_RenderCopy(renderer, _texture, NULL, &rect);
	}
}

SDL_Rect Gem::BuildRectFromParent(SDL_Rect* parent)
{
	SDL_Rect rect = *parent;

	rect.x += _x * (_rect.w + Board::GEM_MARGIN);
	rect.y += _y * (_rect.h + Board::GEM_MARGIN);
	rect.w = _rect.w;
	rect.h = _rect.h;

	return rect;
}

bool Gem::IsMouseInside(SDL_Rect* parent, int mouseX, int mouseY)
{
	if (_id == -1)
		return false;
	
	SDL_Rect rect = BuildRectFromParent(parent);

	if (mouseX < rect.x) return false;
	if (mouseX > rect.x + rect.w) return false;
	if (mouseY < rect.y) return false;
	if (mouseY > rect.y + rect.h) return false;

	return true;
}

void Gem::SetAnimation(Animation animation)
{
	_animation = animation;	
}

Animation Gem::GetAnimation()
{
	return _animation;
}
