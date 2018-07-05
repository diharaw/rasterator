#pragma once

#include <stdio.h>
#include <math.h>
#include <stdint.h>

namespace math
{

	template<typename T>
	struct vec2
	{
		union
		{
			struct
			{
				T x, y;
			};
			T data[2];
		};

		vec2() : x(0.0), y(0.0) {}
		vec2(const T& v) : x(v), y(v) {}
		vec2(const T& _x, const T& _y) : x(_x), y(_y) {}

		friend vec2 operator+(const vec2& lhs, const vec2& rhs)
		{
			return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
		}

		friend vec2 operator-(const vec2& lhs, const vec2& rhs)
		{
			return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
		}

		friend vec2 operator*(const vec2& lhs, const T& rhs)
		{
			return vec2(lhs.x * rhs, lhs.y * rhs);
		}

		friend vec2 operator*(const T& lhs, const vec2& rhs)
		{
			return vec2(lhs * rhs.x, lhs * rhs.y);
		}

		friend vec2 operator/(const vec2& lhs, const T& rhs) 
		{
			return vec2<T>(lhs.x / rhs, lhs.y / rhs);
		}

		inline const T& operator[] (unsigned index) const
		{
			assert(index < 2);
			return data[index];
		}

		inline T& operator[] (unsigned index)
		{
			assert(index < 2);
			return data[index];
		}

		inline float length() const
		{
			return sqrt(x*x + y * y);
		}

		inline float distance(const vec2& v) const
		{
			auto r = *this - v;
			return r.length();
		}

		inline float dot(vec2& b) const
		{
			return x * b.x + y * b.y;
		}

		inline vec2 normalize() const
		{
			auto l = length();
			return vec2(x / l, y / l);
		}

		inline vec2 direction(const vec2& to) const
		{
			auto v = to - *this;
			return v.normalize();
		}

		inline vec2 lerp(const vec2& b, float t) const
		{
			return t < 0.0f ? *this : (t > 1.0f ? b : (*this * (1.0f - t) + b * t));;
		}

		inline void print() const
		{
			printf("[%f, %f]\n", x, y);
		}
	};

	using vec2f = vec2<float>;
	using vec2i = vec2<int32_t>;
	using vec2u = vec2<uint32_t>;

}