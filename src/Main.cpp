#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include "Game.hpp"

static const char* TITLE = "Bejeweled clone by Pedro Pereira";
static const int WIDTH = 1024;
static const int HEIGHT = 768;

SDL_Window* window;
SDL_Renderer* renderer;

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO)) // | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER
	{
		printf("%s\n", SDL_GetError());
		return -1;
	}

	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if(window == NULL)
	{
		printf("%s\n", SDL_GetError());
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("%s\n", SDL_GetError());
		return -1;
	}

	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("%s\n", SDL_GetError());
		return -1;
	}

	Game* game = new Game();
	game->Run(renderer);
	delete game;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();

	return 0;
}