#ifndef RT_VEC3_HEADER
#define RT_VEC3_HEADER

#include <common.h>
#include <xmmintrin.h>

#ifdef _WIN32

#define __vinline __forceinline
#define __vselectany __declspec(selectany)
#define __vget(v, i) m.m128_f32[i]

#else

#define __vinline inline
#define __vselectany __attribute__((selectany,weak))
#define __vget(v, i) m[i]

#endif

struct vconstu
{
    union { uint32_t u[4]; __m128 v; };
    __vinline operator __m128() const { return v; }
};
extern const __vselectany vconstu vsignbits = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };

class Vec3f
{
public:
	__m128 m;
	
public:
	__vinline Vec3f() {}
	__vinline explicit Vec3f(const float *p) { m = _mm_set_ps(p[2], p[2], p[1], p[0]); }
	__vinline explicit Vec3f(float f) { m = _mm_set1_ps(f); }
	__vinline explicit Vec3f(float x, float y, float z) { m = _mm_set_ps(z, z, y, x); }
	__vinline explicit Vec3f(__m128 v) { m = v; }

	__vinline float x() const { return _mm_cvtss_f32(m); }
	__vinline float y() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(1, 1, 1, 1))); }
	__vinline float z() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 2, 2, 2))); }

	__vinline void setX(float x) { m = _mm_move_ss(m, _mm_set_ss(x)); }
	__vinline void setY(float y) 
	{
		__m128 tmp = _mm_move_ss(m, _mm_set_ss(y));
		tmp = _mm_shuffle_ps(tmp, tmp, _MM_SHUFFLE(3, 2, 0, 0));
		m = _mm_move_ss(tmp, m);
	}
	__vinline void setZ(float z)
	{
		__m128 tmp = _mm_move_ss(m, _mm_set_ss(z));
		tmp = _mm_shuffle_ps(tmp, tmp, _MM_SHUFFLE(3, 0, 1, 0));
		m = _mm_move_ss(tmp, m);
	}
	__vinline float operator[] (size_t i) const { return __vget(m,i); };
	__vinline float& operator[] (size_t i) { return __vget(m,i); };

	__vinline static Vec3f random()
	{
		return Vec3f(random_float(), random_float(), random_float());
	}

	__vinline static Vec3f random(float min, float max)
	{
		return Vec3f(random_float(min, max), random_float(min, max), random_float(min, max));
	}

	__vinline Vec3f yzx() const { return Vec3f(_mm_shuffle_ps(m, m, _MM_SHUFFLE(0, 0, 2, 1))); }
	__vinline Vec3f zxy() const { return Vec3f(_mm_shuffle_ps(m, m, _MM_SHUFFLE(1, 1, 0, 2))); }
};

typedef Vec3f Vec3b;

#define SHUFFLE3(V, X,Y,Z) Vec3f(_mm_shuffle_ps((V).m, (V).m, _MM_SHUFFLE(Z,Z,Y,X)))

__vinline Vec3f abs(Vec3f v) 
{ 
	v.m = _mm_andnot_ps(vsignbits, v.m); 
	return v; 
}


__vinline unsigned mask(Vec3f v) 
{
	return _mm_movemask_ps(v.m) & 7; 
}

__vinline bool any(Vec3b v) 
{ 
	return mask(v) != 0; 
}

__vinline bool all(Vec3b v)
{ 
	return mask(v) == 7; 
}

__vinline Vec3f operator+ (Vec3f a, Vec3f b) 
{
	a.m = _mm_add_ps(a.m, b.m); 
	return a; 
}

__vinline Vec3f operator- (Vec3f a, Vec3f b)
{
	a.m = _mm_sub_ps(a.m, b.m);
	return a;
}

__vinline Vec3f operator* (Vec3f a, Vec3f b)
{
	a.m = _mm_mul_ps(a.m, b.m);
	return a;
}

__vinline Vec3f operator/ (Vec3f a, Vec3f b)
{
	a.m = _mm_div_ps(a.m, b.m);
	return a;
}

__vinline Vec3f operator* (Vec3f a, float b)
{
	a.m = _mm_mul_ps(a.m, _mm_set1_ps(b));
	return a;
}

__vinline Vec3f operator* (float a, Vec3f b)
{
	b.m = _mm_mul_ps(_mm_set1_ps(a), b.m);
	return b;
}

__vinline Vec3f operator/ (Vec3f a, float b)
{
	a.m = _mm_div_ps(a.m, _mm_set1_ps(b));
	return a;
}

__vinline Vec3f operator/ (float a, Vec3f b)
{
	b.m = _mm_div_ps(_mm_set1_ps(a), b.m);
	return b;
}

__vinline Vec3f& operator+= (Vec3f &a, Vec3f b) 
{ 
	a = a + b; 
	return a; 
}

__vinline Vec3f& operator-= (Vec3f &a, Vec3f b) 
{ 
	a = a - b; 
	return a; 
}

__vinline Vec3f& operator*= (Vec3f &a, Vec3f b) 
{ 
	a = a * b; 
	return a; 
}

__vinline Vec3f& operator/= (Vec3f &a, Vec3f b) 
{ 
	a = a / b; 
	return a; 
}

__vinline Vec3f& operator*= (Vec3f &a, float b) 
{ 
	a = a * b; 
	return a; 
}

__vinline Vec3f& operator/= (Vec3f &a, float b) 
{ 
	a = a / b; 
	return a; 
}

__vinline Vec3b operator== (Vec3f a, Vec3f b) 
{ 
	a.m = _mm_cmpeq_ps(a.m, b.m); 
	return a; 
}

__vinline Vec3b operator!=(Vec3f a, Vec3f b) 
{ 
	a.m = _mm_cmpneq_ps(a.m, b.m); 
	return a; 
}

__vinline Vec3b operator< (Vec3f a, Vec3f b) 
{ 
	a.m = _mm_cmplt_ps(a.m, b.m); 
	return a;
}

__vinline Vec3b operator> (Vec3f a, Vec3f b) 
{ 
	a.m = _mm_cmpgt_ps(a.m, b.m); 
	return a; 
}

__vinline Vec3b operator<=(Vec3f a, Vec3f b) 
{ 
	a.m = _mm_cmple_ps(a.m, b.m); 
	return a;
}

__vinline Vec3b operator>=(Vec3f a, Vec3f b) 
{ 
	a.m = _mm_cmpge_ps(a.m, b.m); 
	return a; 
}

__vinline Vec3f min(Vec3f a, Vec3f b) 
{ 
	a.m = _mm_min_ps(a.m, b.m); 
	return a; 
}

__vinline Vec3f max(Vec3f a, Vec3f b) 
{ 
	a.m = _mm_max_ps(a.m, b.m); 
	return a; 
}


__vinline float hmin(Vec3f v) {
    v = min(v, SHUFFLE3(v, 1, 0, 2));
    return min(v, SHUFFLE3(v, 2, 0, 1)).x();
}

__vinline float hmax(Vec3f v) {
    v = max(v, SHUFFLE3(v, 1, 0, 2));
    return max(v, SHUFFLE3(v, 2, 0, 1)).x();
}

__vinline Vec3f operator- (Vec3f a)
{
	return Vec3f(_mm_setzero_ps()) - a;
}

#include <iostream>
inline std::ostream& operator << (std::ostream& os, const Vec3f& v)
{
	return os << v.x() << ' ' << v.y() << ' ' << v.z();
}

__vinline float sum(Vec3f a)
{
	return a.x() + a.y() + a.z();
}

__vinline float dot(const Vec3f u, const Vec3f v)
{
	return sum(u * v);
}

__vinline Vec3f cross(Vec3f a, Vec3f b) 
{
	return (a.zxy()*b - a*b.zxy()).zxy();
}

__vinline Vec3f lerp(Vec3f a, Vec3f b, float t)
{
	return a + (b-a) * t;
}

__vinline float length(Vec3f v)
{
	return sqrtf(dot(v, v));
}

__vinline float lengthSq(Vec3f v)
{
	return dot(v, v);
}

__vinline Vec3f normalize(Vec3f v)
{
	return v / length(v);
}

__vinline Vec3f reflect(const Vec3f& v, const Vec3f& n)
{
	return v - (2 * dot(v, n)) * n;
}

__vinline Vec3f refract(const Vec3f& uv, const Vec3f& n, real etai_over_etat)
{
	auto cos_theta = fmin(dot(-uv, n), 1.0);
	Vec3f r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3f r_out_parallel = -sqrt(fabs(1.0 - lengthSq(r_out_perp))) * n;
	return r_out_perp + r_out_parallel;
}

__vinline Vec3f random_vec_in_unit_sphere()
{
	while (true)
	{
		auto p = Vec3f::random(-1, 1);
		if (lengthSq(p) < 1)
			return p;
	}
}

__vinline Vec3f random_vec_in_unit_disk()
{
	while (true)
	{
		auto p = Vec3f(random_float(-1, 1), random_float(-1, 1), 0.0);
		if (lengthSq(p) < 1)
			return p;
	}
}

__vinline bool near_zero(Vec3f v)
{
	const auto eps = Vec3f(_mm_set1_ps(1e-8));
	return all(abs(v) < eps);
}

#endif
