#include "GemFallAnimation.hpp"

GemFallAnimation::GemFallAnimation(Pair start, Pair end) : Animation()
{
	InsertFrame(0, start, 1, 1);
	InsertFrame(.5f, end, 1, 1);
}