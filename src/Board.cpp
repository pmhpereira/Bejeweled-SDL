#include "Board.hpp"

#include <SDL_image.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>

#include "Animation\GemDestroyAnimation.hpp"
#include "Animation\GemFallAnimation.hpp"
#include "Animation\GemSwitchAnimation.hpp"

const int Board::COLUMNS = 8;
const int Board::ROWS = 8;

const int Board::GEM_SIZE = 70;
const int Board::GEM_MARGIN = 5;
const int Board::GEM_TYPES = 5;

const int Board::MIN_SEQUENCE = 3;

static const std::string FILENAME = "Board.txt";

Gem* firstGem, *secondGem;
std::vector<Gem*> gemsAnimating;

void Board::PrepareSelectedGemTexture()
{
	std::string path = "Selected.png";
	SDL_Surface* loadingSurface = IMG_Load(path.c_str());
	if (loadingSurface == NULL)
	{
		printf("%s\n", IMG_GetError());
		return;
	}

	_selectedTexture = SDL_CreateTextureFromSurface(_renderer, loadingSurface);
	SDL_FreeSurface(loadingSurface);
	loadingSurface = NULL;
}

void Board::PrepareDefaultGemSurfaces()
{
	for (int i = 0; i < GEM_TYPES; i++)
	{
		std::stringstream ss;
		ss << i;

		std::string path = "Color-" + ss.str() + ".png";
		SDL_Surface* loadingSurface = IMG_Load(path.c_str());
		if (loadingSurface == NULL)
		{
			printf("%s\n", IMG_GetError());
			return;
		}

		_defaultGemSurfaces.push_back(loadingSurface);
	}
}

void Board::Init(SDL_Renderer* renderer)
{
	_renderer = renderer;

	PrepareSelectedGemTexture();
	PrepareDefaultGemSurfaces();

	_rect = new SDL_Rect();
	_rect->w = 1024;
	_rect->h = 768;
	_rect->x = 343;
	_rect->y = 87;

	_columns = COLUMNS;
	_rows = ROWS;

	_gems = new Gem*[_columns];
	for (int x = 0; x < _columns; x++)
	{
		_gems[x] = new Gem[_rows];
	}

	Reset();
}

void Board::Reset()
{
	firstGem = NULL;
	secondGem = NULL;

	for (int x = 0; x < _columns; x++)
	{
		for (int y = 0; y < _rows; y++)
		{
			_gems[x][y] = Gem();
		}
	}

	_state = BoardState::Idle;
	Fill();
}

void Board::Update(SDL_Event* e)
{
	// check board state and update animations
	switch (_state)
	{
	case BoardState::Switching:
	case BoardState::SwitchingBack:
		SwitchGems(gemsAnimating[0]->GetBoardPosition(), gemsAnimating[1]->GetBoardPosition());
		break;
	case BoardState::Destroying:
		Evaluate();
		break;
	case BoardState::Packing:
		Pack();
		break;
	case BoardState::Filling:
		Fill();
		break;
	}

	if (_state != BoardState::Idle)
		return;

	if (e != NULL)
	{
		if (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			Gem* clickedGem = GetGemAtMousePosition(x, y);

			if (e->type == SDL_MOUSEBUTTONDOWN) // mouse click
			{
				if (clickedGem == NULL)
				{
					firstGem = NULL;
				}
				else
				{
					if (firstGem == NULL)
					{
						firstGem = clickedGem;
					}
					else
					{
						secondGem = clickedGem;
					}
				}
			}
			else if (e->type == SDL_MOUSEBUTTONUP) // mouse drag
			{
				if (clickedGem != NULL && firstGem != NULL && firstGem != clickedGem)
				{
					secondGem = clickedGem;
				}
			}
		}
	}

	// switch gems or reselect first gem
	if (firstGem != NULL && secondGem != NULL)
	{
		if (!SwitchGems(firstGem->GetBoardPosition(), secondGem->GetBoardPosition()))
		{
			firstGem = secondGem;
			secondGem = NULL;
		}
		else
		{
			firstGem = NULL;
			secondGem = NULL;
		}
	}
}

void Board::Draw()
{
	// draw all the gems in the board
	for (int x = 0; x < _columns; x++)
	{
		for (int y = 0; y < _rows; y++)
		{
			_gems[x][y].Draw(_renderer, _rect);
		}
	}

	// show first gem selected
	if (firstGem != NULL)
	{
		SDL_RenderCopy(_renderer, _selectedTexture, NULL, &firstGem->BuildRectFromParent(_rect));
	}
}

bool Board::Evaluate()
{
	/*
	state Evaluating: checks what gems to remove
	state Destroying: removes gems and waits for GemDestroyAnimation to finish
	*/

	bool returnValue = false;

	if (_state == BoardState::Evaluating)
	{
		int lastID = -1;
		int sequenceLength = 0;
		Pair start;

		std::vector<Pair> positionsToRemove;

		// checks for horizontal sequences
		for (int y = 0; y < _rows; y++)
		{
			start = Pair(0, y);
			lastID = -1;
			sequenceLength = 0;

			for (int x = 0; x < _columns; x++)
			{
				if (_gems[x][y].GetID() == lastID)
				{
					sequenceLength++;

					// if we reached the last column and have a sequence 
					if (x == _columns - 1 && sequenceLength >= MIN_SEQUENCE && lastID != -1)
					{
						returnValue = true;

						for (int _x = start.x; _x < start.x + sequenceLength; _x++)
						{
							positionsToRemove.push_back(Pair(_x, start.y));
						}
					}
				}
				else
				{
					// if we found a sequence in the middle columns
					if (sequenceLength >= MIN_SEQUENCE && lastID != -1)
					{
						returnValue = true;

						for (int _x = start.x; _x < start.x + sequenceLength; _x++)
						{
							positionsToRemove.push_back(Pair(_x, start.y));
						}
					}

					lastID = _gems[x][y].GetID();
					sequenceLength = 1;
					start = Pair(x, y);
				}
			}
		}

		// checks for vertical sequences
		for (int x = 0; x < _columns; x++)
		{
			start = Pair(x, 0);
			lastID = -1;
			sequenceLength = 0;

			for (int y = 0; y < _rows; y++)
			{
				if (_gems[x][y].GetID() == lastID)
				{
					sequenceLength++;

					// if we reached the last row and have a sequence 
					if (y == _rows - 1 && sequenceLength >= MIN_SEQUENCE && lastID >= 0)
					{
						returnValue = true;

						for (int _y = start.y; _y < start.y + sequenceLength; _y++)
						{
							positionsToRemove.push_back(Pair(start.x, _y));
						}
					}
				}
				else
				{
					// if we found a sequence in the middle rows
					if (sequenceLength >= MIN_SEQUENCE && lastID >= 0)
					{
						returnValue = true;

						for (int _y = start.y; _y < start.y + sequenceLength; _y++)
						{
							positionsToRemove.push_back(Pair(start.x, _y));
						}
					}

					lastID = _gems[x][y].GetID();
					sequenceLength = 1;
					start = Pair(x, y);
				}
			}
		}

		// traverse gems to remove and set their GemDestroyAnimation
		for (unsigned int i = 0; i < positionsToRemove.size(); i++)
		{
			_state = BoardState::Destroying;

			Pair position = positionsToRemove[i];
			SDL_Rect rect = _gems[position.x][position.y].BuildRectFromParent(_rect);
			_gems[position.x][position.y].SetAnimation(GemDestroyAnimation(Pair(rect.x, rect.y)));
			gemsAnimating.push_back(&_gems[position.x][position.y]);
		}
	}
	else if (_state == BoardState::Destroying)
	{
		if (gemsAnimating.size() > 0)
		{
			// wait for GemDestroyAnimation to end
			if (gemsAnimating[0]->GetAnimation().HasEnded())
			{
				// replace destroy gems for empty ones
				for (unsigned int i = 0; i < gemsAnimating.size(); i++)
				{
					Pair position = gemsAnimating[i]->GetBoardPosition();

					_gems[position.x][position.y] = Gem();
					_gems[position.x][position.y].SetPosition(position.x, position.y);
				}

				gemsAnimating.clear();

				// try to pack the board
				_state = BoardState::Idle;
				Pack();
			}
		}

		return true;
	}

	if (!returnValue)
		_state = BoardState::Idle;

	return returnValue;
}

void Board::Pack()
{
	/*
	state Idle: checks what gems to move down
	state Destroying: moves gems and waits for GemFallAnimation to finish
	*/
	if (_state == BoardState::Idle)
	{
		// traverse all board
		for (int x = 0; x < _columns; x++)
		{
			for (int y = _rows - 1; y >= 0; y--)
			{
				// if it is an empty gem
				if (_gems[x][y].GetID() < 0)
				{
					for (int ny = y - 1; ny >= 0; ny--)
					{
						// search for the first colored gem above
						if (_gems[x][ny].GetID() >= 0)
						{
							// set the GemFallAnimation for the colored gem
							SDL_Rect startRect = _gems[x][ny].BuildRectFromParent(_rect);
							SDL_Rect endRect = _gems[x][y].BuildRectFromParent(_rect);
							_gems[x][ny].SetAnimation(GemFallAnimation(Pair(startRect.x, startRect.y), Pair(endRect.x, endRect.y)));

							gemsAnimating.push_back(&_gems[x][ny]);

							_gems[x][y] = _gems[x][ny];
							_gems[x][y].SetPosition(x, y);

							_gems[x][ny] = Gem();
							_gems[x][ny].SetPosition(x, ny);

							ny = -1;
						}
					}
				}
			}
		}

		_state = BoardState::Packing;
	}
	else if (_state == BoardState::Packing)
	{
		if (gemsAnimating.size() > 0)
		{
			// wait for GemFallAnimation to end
			if (gemsAnimating[0]->GetAnimation().HasEnded())
			{
				gemsAnimating.clear();
			}
		}

		// try to fill the board
		_state = BoardState::Idle;
		Fill();
	}
}

void Board::Fill()
{
	/*
	state Idle: checks what positions to fill
	state Filling: fills gems and waits for GemFallAnimation to finish
	*/
	if (_state == BoardState::Idle)
	{
		// traverse the board
		for (int x = 0; x < _columns; x++)
		{
			int yLowest = INT_MIN;

			for (int y = _rows; y >= 0; y--)
			{
				// if is an empty gem
				if (_gems[x][y].GetID() < 0)
				{
					// save the downest empty row position
					if (yLowest == INT_MIN)
					{
						yLowest = y;
					}

					int gemID;

					// randomly check a gem to fill the position
					do
					{
						gemID = rand() % Board::GEM_TYPES;
					} while (!CanPlaceGem(gemID, x, y));

					// create the new gem
					_gems[x][y] = Gem(gemID, _renderer, _defaultGemSurfaces[gemID]);
					_gems[x][y].SetPosition(x, y - yLowest - 1);
					SDL_Rect startRect = _gems[x][y].BuildRectFromParent(_rect);

					// set the GemFallAnimation
					_gems[x][y].SetPosition(x, y);
					SDL_Rect endRect = _gems[x][y].BuildRectFromParent(_rect);
					_gems[x][y].SetAnimation(GemFallAnimation(Pair(startRect.x, startRect.y), Pair(endRect.x, endRect.y)));

					_state = BoardState::Filling;
					gemsAnimating.push_back(&_gems[x][y]);
				}
			}
		}
	}
	else if (_state == BoardState::Filling)
	{
		if (gemsAnimating.size() > 0)
		{
			// wait for GemFallAnimation to end
			if (gemsAnimating[0]->GetAnimation().HasEnded())
			{
				gemsAnimating.clear();

				// re-evaluate the board to find new sequences
				_state = BoardState::Evaluating;
				Evaluate();
			}
		}
	}
}

Gem* Board::GetGemAtMousePosition(int mouseX, int mouseY)
{
	// traverse all gems in the board
	for (int x = 0; x < _columns; x++)
	{
		for (int y = 0; y < _rows; y++)
		{
			// return the gem that the mouse is touching
			if (_gems[x][y].IsMouseInside(_rect, mouseX, mouseY))
			{
				return &_gems[x][y];
			}
		}
	}

	return NULL;
}

bool Board::CanPlaceGem(int gemID, int boardX, int boardY)
{
	int sequenceLength = 1;

	// check for possible horizontal sequences starting from the given position
	for (int x = boardX - 1; x >= 0; x--)
	{
		if (_gems[x][boardY].GetID() == gemID)
		{
			if (++sequenceLength >= MIN_SEQUENCE)
			{
				return false;
			}
		}
		else
		{
			break;
		}
	}

	sequenceLength = 1;

	// check for possible horizontal sequences starting from the given position
	for (int y = boardY + 1; y < _rows; y++)
	{
		if (_gems[boardX][y].GetID() == gemID)
		{
			if (++sequenceLength >= MIN_SEQUENCE)
			{
				return false;
			}
		}
		else
		{
			break;
		}
	}

	return true;
}

bool Board::SwitchGems(Pair first, Pair second)
{
	/*
	state Idle: checks if two gems are switchable
	state Switching: switches gems and waits for GemSwitchAnimation to finish
	state SwitchingBack: switches gems back and waits for GemFallAnimation to finish
	*/
	if (_state == BoardState::Idle)
	{
		// calculate the absolute distance between the two gems
		int dx = (int)fabs(first.x - second.x);
		int dy = (int)fabs(first.y - second.y);

		// if the gems are directly adjacent
		if (dx <= 1 && dy <= 1 && dx + dy == 1)
		{
			// set the GemSwitchAnimation for both of them
			SDL_Rect firstRect = _gems[first.x][first.y].BuildRectFromParent(_rect);
			Pair firstPair(firstRect.x, firstRect.y);

			SDL_Rect secondRect = _gems[second.x][second.y].BuildRectFromParent(_rect);
			Pair secondPair(secondRect.x, secondRect.y);

			_gems[first.x][first.y].SetAnimation(GemSwitchAnimation(firstPair, secondPair));
			_gems[second.x][second.y].SetAnimation(GemSwitchAnimation(secondPair, firstPair));

			gemsAnimating.push_back(&_gems[first.x][first.y]);
			gemsAnimating.push_back(&_gems[second.x][second.y]);

			_state = BoardState::Switching;

			return true;
		}
	}
	else if (_state == BoardState::Switching)
	{
		if (gemsAnimating.size() > 0)
		{
			// wait for GemSwitchAnimation to finish
			if (gemsAnimating[0]->GetAnimation().HasEnded())
			{
				gemsAnimating.clear();

				// update the position of the switched gems
				Gem aux = _gems[first.x][first.y];
				_gems[first.x][first.y] = _gems[second.x][second.y];
				_gems[first.x][first.y].SetPosition(first.x, first.y);

				_gems[second.x][second.y] = aux;
				_gems[second.x][second.y].SetPosition(second.x, second.y);

				_state = BoardState::Evaluating;

				// evaluate the board
				// if the switching didn't create sequences
				if (!Evaluate())
				{
					// switch the gems back
					SDL_Rect firstRect = _gems[first.x][first.y].BuildRectFromParent(_rect);
					Pair firstPair(firstRect.x, firstRect.y);

					SDL_Rect secondRect = _gems[second.x][second.y].BuildRectFromParent(_rect);
					Pair secondPair(secondRect.x, secondRect.y);

					_gems[first.x][first.y].SetAnimation(GemSwitchAnimation(firstPair, secondPair));
					_gems[second.x][second.y].SetAnimation(GemSwitchAnimation(secondPair, firstPair));

					gemsAnimating.push_back(&_gems[first.x][first.y]);
					gemsAnimating.push_back(&_gems[second.x][second.y]);

					_state = BoardState::SwitchingBack;
				}
			}
		}
	}
	else if (_state == BoardState::SwitchingBack)
	{
		if (gemsAnimating.size() > 0)
		{
			// wait for GemSwitchAnimation to finish
			if (gemsAnimating[0]->GetAnimation().HasEnded())
			{
				gemsAnimating.clear();

				// reset the position of the gems
				Gem aux = _gems[first.x][first.y];
				_gems[first.x][first.y] = _gems[second.x][second.y];
				_gems[first.x][first.y].SetPosition(first.x, first.y);

				_gems[second.x][second.y] = aux;
				_gems[second.x][second.y].SetPosition(second.x, second.y);

				_state = BoardState::Idle;
			}
		}
	}

	return false;
}

void Board::SaveToFile()
{
	std::ofstream boardFile(FILENAME);

	if (boardFile.is_open())
	{
		boardFile << _columns << " " << _rows << std::endl;

		for (int y = 0; y < _rows; y++)
		{
			for (int x = 0; x < _columns; x++)
			{
				if (_gems[x][y].GetID() == -1)
					boardFile << "-";
				else
					boardFile << _gems[x][y].GetID();
			}

			boardFile << std::endl;
		}

		boardFile.close();
	}
}

void Board::LoadFromFile()
{
	std::ifstream boardFile(FILENAME);
	std::string line;

	if (boardFile.is_open())
	{
		if (getline(boardFile, line))
		{
			std::string val;

			val = line.substr(0, line.find(" "));
			_columns = std::stoi(val);
			val = line.substr(line.find(" "));
			_rows = std::stoi(val);

			for (int y = 0; y < _rows; y++)
			{
				if (!getline(boardFile, line))
					return;

				for (int x = 0; x < _columns; x++)
				{
					val = line[x];

					if (val.compare("-") == 0)
						_gems[x][y] = Gem();
					else
					{
						int gemID = std::stoi(val);
						_gems[x][y] = Gem(gemID, _renderer, _defaultGemSurfaces[gemID]);
					}

					_gems[x][y].SetPosition(x, y);
				}
			}
		}

		boardFile.close();
	}
}

Board::~Board()
{
	delete _rect;
	delete[] _gems;
}