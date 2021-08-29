#pragma once

#include <Color.h>

class MSColor : public Color
{
public:
	uint32_t sample_count;

public:
	MSColor() : Color(), sample_count(0) {}
	MSColor(real rgb) : Color(rgb), sample_count(1) {}
	MSColor(real r, real g, real b): Color(r, g, b), sample_count(1) {}
	
	inline MSColor& operator += (const Color& v) 
	{
		data[0] += v.data[0];
		data[1] += v.data[1];
		data[2] += v.data[2];
		sample_count++;
		return *this;
	}

	void reduce();
};

inline MSColor operator + (const MSColor& u, const MSColor& v)
{
	MSColor retval = MSColor{
		u.data[0] + v.data[0],
		u.data[1] + v.data[1],
		u.data[2] + v.data[2]
	};
	retval.sample_count = u.sample_count + v.sample_count;

	return retval;
}
