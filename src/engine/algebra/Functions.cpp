#include "Functions.hpp"
#include "engine/core/msys_temp.hpp"

using namespace Algebra;

float Algebra::abs(float x)
{
	return std::abs(x);
}

// Code from Iñigo Quilez's 64k demo framework.
// http://iquilezles.org/code/framework64k/framework64k.htm
float Algebra::floor(float x)
{
	union
	{
		float	f;
		int		i;
	} val;
	val.f = x;
	int exponent = (val.i >> 23) & 0xff; // extract the exponent field;
	int fractional_bits = 127 + 23 - exponent;
	if (fractional_bits > 23) // abs(x) < 1.0
	{
		return 0.0;
	}
	if (fractional_bits > 0)
	{
		val.i &= ~((1U << fractional_bits) - 1);
	}
	return val.f;
}

float Algebra::fract(float x)
{
	return x - Algebra::floor(x);
}

// Code from Iñigo Quilez's 64k demo framework.
// http://iquilezles.org/code/framework64k/framework64k.htm
float Algebra::mod(float x, float y)
{
	float res;

#ifdef _WIN32
	_asm fld     dword ptr [y]
	_asm fld     dword ptr [x]
	_asm fprem
	_asm fxch    st(1)
	_asm fstp    st(0)
	_asm fstp    dword ptr [res]
#else // !_WIN32
	res = fmodf(x, y);
#endif // !_WIN32

	return res;
}

// Code from Iñigo Quilez's 64k demo framework.
// http://iquilezles.org/code/framework64k/framework64k.htm
float Algebra::pow(float x, float y)
{
	if (x == 0.f)
	{
		return 0.f;
	}

	float res;

#ifdef _WIN32
	_asm fld     dword ptr [y]
	_asm fld     dword ptr [x]
	_asm fyl2x
	_asm fld1
	_asm fld     st(1)
	_asm fprem
	_asm f2xm1
	_asm faddp   st(1), st(0)
	_asm fscale
	_asm fxch
	_asm fstp    st(0)
	_asm fstp    dword ptr [res];
#else // !_WIN32
	res = std::pow(x, y);
#endif // !_WIN32

	return res;
}
