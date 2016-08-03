#include "Game.hpp"
#include <SDL_image.h>

#include <time.h>

const int Game::TARGET_FPS = 60;

void Game::Init(SDL_Renderer* renderer)
{
	srand((unsigned int) time(NULL));

	_renderer = renderer;

	_board = new Board();
	_board->Init(_renderer);

	std::string path = "Background.jpg";
	SDL_Surface* loadingSurface = IMG_Load(path.c_str());
	if (loadingSurface == NULL)
	{
		printf("%s\n", IMG_GetError());
		return;
	}

	_backgroundTexture = SDL_CreateTextureFromSurface(_renderer, loadingSurface);
	SDL_FreeSurface(loadingSurface);
	loadingSurface = NULL;
}

void Game::Run(SDL_Renderer* renderer)
{
	Init(renderer);

	SDL_Event e;

	while (true)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				return;
			}
	
			Update(&e);
		}

		Update(NULL);
		Draw();
	}
}

void Game::Update(SDL_Event* e)
{
	if (e != NULL)
	{
		if(e->type == SDL_KEYUP)
		{
			switch (e->key.keysym.sym)
			{
			case SDLK_r:
				Reset();
				break;

			case SDLK_s:
				Save();
				break;

			case SDLK_l:
				Load();
				break;
			}
		}
	}

	_board->Update(e);
}

void Game::Draw()
{
	SDL_RenderClear(_renderer);
	SDL_RenderCopy(_renderer, _backgroundTexture, NULL, NULL);
	_board->Draw();

	SDL_RenderPresent(_renderer);

	SDL_Delay(1000 / TARGET_FPS);
}

void Game::Reset()
{
	_board->Reset();
}

void Game::Save()
{
	_board->SaveToFile();
}

void Game::Load()
{
	_board->LoadFromFile();
}

Game::~Game()
{
	delete _board;
}