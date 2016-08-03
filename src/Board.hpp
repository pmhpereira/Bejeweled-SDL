#pragma once

#include <vector>

#include "Gem.hpp"
#include "Pair.hpp"

enum BoardState
{
	Idle,
	Selecting,
	Switching,
	SwitchingBack,
	Evaluating,
	Destroying,
	Packing,
	Filling,
};

class Board
{
private:
	SDL_Renderer* _renderer;

	SDL_Texture* _selectedTexture;
	std::vector<SDL_Surface*> _defaultGemSurfaces;

	int _columns, _rows;
	SDL_Rect* _rect;
	
	BoardState _state;
	Gem** _gems;

	void PrepareSelectedGemTexture();
	void PrepareDefaultGemSurfaces();
	
	Gem* GetGemAtMousePosition(int, int);
	bool CanPlaceGem(int, int, int);
	bool SwitchGems(Pair, Pair);

	bool Evaluate();
	void Pack();
	void Fill();

public:
	static const int ROWS;
	static const int COLUMNS;

	static const int GEM_SIZE;
	static const int GEM_MARGIN;
	static const int GEM_TYPES;

	static const int MIN_SEQUENCE;

	void Init(SDL_Renderer*);
	void Reset();

	void Update(SDL_Event* e);
	void Draw();

	void SaveToFile();
	void LoadFromFile();

	~Board();
};
