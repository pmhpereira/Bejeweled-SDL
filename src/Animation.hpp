#pragma once

#include <vector>

#include <SDL.h>

#include "Pair.hpp"

 class Animation
{
private:
	int _startAnimationTicks;
	int _startFrameTicks;
	float _time;

	bool _started;

	int _index, _nextIndex;

	std::vector<float> _times;
	std::vector<Pair> _positions;
	std::vector<float> _scales;
	std::vector<float> _alphas;

	int GetNextIndex();

public:
	Animation();

	void InsertFrame(float, Pair, float, float);

	void Update();
	bool HasStarted();
	bool HasEnded();

	Pair GetPosition();
	float GetScale();
	float GetAlpha();
};