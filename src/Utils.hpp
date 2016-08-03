#pragma once

#include "Pair.hpp"

class Utils
{
public:
	static float Lerp(float t, float a, float b)
	{
		return (1 - t)*a + t*b;
	}

	static Pair Lerp(float t, Pair a, Pair b)
	{
		Pair newPair;
		newPair.x = (int)((1 - t)*a.x + t*b.x);
		newPair.y = (int)((1 - t)*a.y + t*b.y);
		return newPair;
	}
};