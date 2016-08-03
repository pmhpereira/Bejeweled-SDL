#include "GemSwitchAnimation.hpp"

GemSwitchAnimation::GemSwitchAnimation(Pair first, Pair second) : Animation()
{
	InsertFrame(0, first, 1, 1);
	InsertFrame(.5f, second, 1, 1);
}