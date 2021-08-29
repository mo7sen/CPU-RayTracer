#include "MSColor.h"

void MSColor::reduce()
{
	real scale = 1.0 / sample_count;
	data[0] = clamp_f(sqrt(data[0] * scale), 0.0, 0.999);
	data[1] = clamp_f(sqrt(data[1] * scale), 0.0, 0.999);
	data[2] = clamp_f(sqrt(data[2] * scale), 0.0, 0.999);
	sample_count = 1;
}
