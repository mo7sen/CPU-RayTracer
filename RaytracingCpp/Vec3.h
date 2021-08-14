#ifndef RT_VEC3_HEADER
#define RT_VEC3_HEADER

#include "common.h"

class Vec3
{
public:
	real data[3];

public:
	Vec3() : Vec3( 0, 0, 0 ) {}
	Vec3(real t) : Vec3( t, t, t ) {}
	Vec3(real x, real y, real z) : data{ x, y, z } {}

	real x() const { return data[0]; }
	real y() const { return data[1]; }
	real z() const { return data[2]; }

	Vec3 operator-() const { return Vec3{ -data[0], -data[1], -data[2] }; }
	real operator[](int i) const { return data[i]; }
	real& operator[](int i) { return data[i]; }

	inline static Vec3 random()
	{
		return Vec3(random_double(), random_double(), random_double());
	}

	inline static Vec3 random(double min, double max)
	{
		return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	bool near_zero() const;

	inline Vec3& operator += (const Vec3& v) 
	{
		data[0] += v.data[0];
		data[1] += v.data[1];
		data[2] += v.data[2];
		return *this;
	}

	inline Vec3& operator *= (const real t)
	{
		data[0] *= t;
		data[1] *= t;
		data[2] *= t;
		return *this;
	}

	inline Vec3& operator /= (const real t)
	{
		return *this *= real(1.0) / t;
	}

	real length() const;
	real length_squared() const;
};

#include <iostream>
inline std::ostream& operator << (std::ostream& os, const Vec3& v)
{
	return os << v.data[0] << ' ' << v.data[1] << ' ' << v.data[2];
}

inline Vec3 operator + (const Vec3& u, const Vec3& v)
{
	return Vec3{
		u.data[0] + v.data[0],
		u.data[1] + v.data[1],
		u.data[2] + v.data[2]
	};
}
inline Vec3 operator - (const Vec3& u, const Vec3& v)
{
	return Vec3{
		u.data[0] - v.data[0],
		u.data[1] - v.data[1],
		u.data[2] - v.data[2]
	};
}
inline Vec3 operator * (const Vec3& u, const Vec3& v)
{
	return Vec3{
		u.data[0] * v.data[0],
		u.data[1] * v.data[1],
		u.data[2] * v.data[2]
	};
}

inline Vec3 operator * (real s, const Vec3& v)
{
	return Vec3{
		v.data[0] * s,
		v.data[1] * s,
		v.data[2] * s
	};
}

inline Vec3 operator * (const Vec3& v, real s)
{
	return s * v;
}

inline Vec3 operator / (const Vec3 &v, const real t) 
{
	return (1 / t) * v;
}

inline real dot(const Vec3& u, const Vec3& v)
{
	return 
		u.data[0] * v.data[0] + 
		u.data[1] * v.data[1] + 
		u.data[2] * v.data[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) 
{
    return Vec3{
		u.data[1] * v.data[2] - u.data[2] * v.data[1],
		u.data[2] * v.data[0] - u.data[0] * v.data[2],
		u.data[0] * v.data[1] - u.data[1] * v.data[0]
	};
}

inline Vec3 unit_vector(Vec3 v)
{
	return v / v.length();
}

inline Vec3 reflect(const Vec3& v, const Vec3& n)
{
	return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(const Vec3& uv, const Vec3& n, real etai_over_etat)
{
	auto cos_theta = fmin(dot(-uv, n), 1.0);
	Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}

inline Vec3 random_vec_in_unit_sphere()
{
	while (true)
	{
		auto p = Vec3::random(-1, 1);
		if (p.length_squared() < 1)
			return p;
	}
}

inline Vec3 random_vec_in_unit_disk()
{
	while (true)
	{
		auto p = Vec3(random_double(-1, 1), random_double(-1, 1), 0.0);
		if (p.length_squared() < 1)
			return p;
	}
}

#endif
