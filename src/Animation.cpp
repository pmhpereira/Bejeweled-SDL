#include "Animation.hpp"
#include "Utils.hpp"

Animation::Animation()
{
	_started = false;
	Update();
}

void Animation::InsertFrame(float time, Pair position, float scale, float alpha)
{
	_times.push_back(time);
	_positions.push_back(position);
	_scales.push_back(scale);
	_alphas.push_back(alpha);
}

void Animation::Update()
{
	int ticks = SDL_GetTicks();

	if (!_started)
	{
		_started = true;
		_startFrameTicks = ticks;
		_startAnimationTicks = ticks;
		_index = 0;
		_time = 0;
	}

	_time = (SDL_GetTicks() - _startAnimationTicks) / 1000.f;

	_nextIndex = GetNextIndex();

	// if the animation needs the next frame
	if (_nextIndex >= 0 && (int)_times.size() > _nextIndex && _time > _times[_nextIndex])
	{
		_startFrameTicks = ticks;
		_index = _nextIndex;
		_nextIndex = GetNextIndex();
	}
}

bool Animation::HasStarted()
{
	return _started;
}

bool Animation::HasEnded()
{
	return _times.size() == 0 || _time >= _times[_times.size() - 1];
}

int Animation::GetNextIndex()
{
	if ((int)_times.size() > _index + 1)
	{
		return _index + 1;
	}

	return _times.size() - 1;
}

Pair Animation::GetPosition()
{
	return Utils::Lerp((SDL_GetTicks() - _startFrameTicks) / (_times[_nextIndex] * 1000), _positions[_index], _positions[_nextIndex]);
}

float Animation::GetScale()
{
	return Utils::Lerp((SDL_GetTicks() - _startFrameTicks) / (_times[_nextIndex] * 1000), _scales[_index], _scales[_nextIndex]);
}

float Animation::GetAlpha()
{
	return Utils::Lerp((SDL_GetTicks() - _startFrameTicks) / (_times[_nextIndex] * 1000), _alphas[_index], _alphas[_nextIndex]);
}
