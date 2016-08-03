#pragma once

#include "Board.hpp"

class Game
{
private:
	SDL_Renderer* _renderer;
	SDL_Texture* _backgroundTexture;

	Board* _board;

	void Init(SDL_Renderer*);
	void Update(SDL_Event* e);
	void Draw();

	void Reset();
	void Save();
	void Load();

public:
	static const int TARGET_FPS;

	void Run(SDL_Renderer*);

	~Game();
};