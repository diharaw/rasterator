#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

namespace math
{
	inline float radians(const float& _degrees)
	{
		return (_degrees * M_PI) / 180.0f;
	}

	inline float degrees(const float& _radians)
	{
		return (_radians * 180.0f) / float(M_PI);
	}

	inline float lerp(float a, float b, float t)
	{
		return t < 0.0f ? a : (t > 1.0f ? b : (a * (1.0f - t) + b * t));
	}
}
