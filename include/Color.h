#ifndef RT_COLOR_HEADER
#define RT_COLOR_HEADER

#include <common.h>
#include <Vec3.h>

class Color {
public:
	real data[4];
public:
	Color() = default;
	Color(real rgb) : Color(rgb, rgb, rgb) {}
	Color(real r, real g, real b) : data{ r, g, b, 1.0 } {}

	static Color fromUnitVector(const Vec3f& v);

	real r() const { return data[0]; }
	real g() const { return data[1]; }
	real b() const { return data[2]; }

	uint32_t r_u8() const { return static_cast<uint32_t>(255.999 * data[0]); }
	uint32_t g_u8() const { return static_cast<uint32_t>(255.999 * data[1]); }
	uint32_t b_u8() const { return static_cast<uint32_t>(255.999 * data[2]); }

	void set(real r, real g, real b) { data[0] = r; data[1] = g; data[2] = b; }

	~Color() = default;
};

#include <iostream>
inline std::ostream& operator<<(std::ostream& os, Color const& color)
{ 
	os << color.r_u8() << ' ' << color.g_u8() << ' ' << color.b_u8();
	return os;
}

inline Color operator * (real s, const Color& c)
{
	return Color{
		c.data[0] * s,
		c.data[1] * s,
		c.data[2] * s
	};
}

inline Color operator * (const Color& c, real s)
{
	return s * c;
}

inline Color operator + (const Color& u, const Color& v)
{
	return Color{
		u.data[0] + v.data[0],
		u.data[1] + v.data[1],
		u.data[2] + v.data[2]
	};
}
inline Color operator - (const Color& u, const Color& v)
{
	return Color{
		u.data[0] - v.data[0],
		u.data[1] - v.data[1],
		u.data[2] - v.data[2]
	};
}
inline Color operator * (const Color& u, const Color& v)
{
	return Color{
		u.data[0] * v.data[0],
		u.data[1] * v.data[1],
		u.data[2] * v.data[2]
	};
}
#endif
