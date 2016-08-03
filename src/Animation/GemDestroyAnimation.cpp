#include "GemDestroyAnimation.hpp"

GemDestroyAnimation::GemDestroyAnimation(Pair position) : Animation()
{
	InsertFrame(0, position, 1, 1);
	InsertFrame(.5f, position, 0.7f, 0);
}