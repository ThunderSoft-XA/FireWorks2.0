/*
 * math_aux.h
 *
 *  Created on: Nov 21, 2019
 *      Author: little
 */

#ifndef INCLUDE_MATH_AUX_H_
#define INCLUDE_MATH_AUX_H_

#include <math.h>

#include "gl_aux.h"


static auto ZeroCenteredMapping = [](float v, float range, bool normalized = false)
{
	GLfloat f = (v - range / 2);
	if (normalized)	f /= (range / 2);
	return f;
};



static auto Normalize = [](float v, float min_v, float max_v, bool positive = true)
{
	if (max_v <= min_v) return 0.f;

	if (positive)
	{
		return (v - min_v) / (max_v - min_v);
	}
	else
	{
		return ZeroCenteredMapping(v, max_v - min_v, true);
	}
};



auto Random = [](unsigned int R = 0x10000)
{
	return (float)(rand() & (R - 1)) / (float)R;
};




// Euclidean distance
static auto Distance = [](float x, float y, float x0 = 0.f, float y0 = 0.f)
{
	double dx = (double)(x - x0);
	double dy = (double)(y - y0);

	return sqrt(dx * dx + dy * dy);
};



auto Sine = [](double omega, double phase, double t, bool positive = true)
{
	double v = sin(omega * t + phase);

	return positive ? (v + 1.0) / 2.0 : v;
};



// Three-dimension sinc function
static auto Sinc = [](float x, float y)
{
	float v;

	if (0.f == x and 0.f == y)
	{
		v = 1.f;
	}
	else if (0.f == x)
	{
		v = sin(y) / y;
	}
	else if (0.f == y)
	{
		v = sin(x) / x;
	}
	else
	{
		v = (sin(x) / x) * (sin(y) / y);
	}

	return v;
};



#endif /* INCLUDE_MATH_AUX_H_ */
