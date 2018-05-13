#pragma once

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

namespace math
{

	template<typename T>
	struct vec4
	{
		union
		{
			struct
			{
				T x, y, z, w;
			};
			T data[4];
		};

		vec4() : x(0.0), y(0.0), z(0.0), w(0.0) {}
		vec4(const T v) : x(v), y(v), z(v), w(v) {}
		vec4(const T& _x, const T& _y, const T& _z, const T& _w) : x(_x), y(_y), z(_z), w(_w) {}

		inline vec4 operator+(const vec4& other) const
		{
			return vec4(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		inline vec4 operator-(const vec4& other) const
		{
			return vec4(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		inline vec4 operator*(const T& s) const
		{
			return vec4(x * s, y * s, z * s, w * s);
		}

		inline vec4 operator/(const T& s) const
		{
			return vec4(x / s, y / s, z / s, w / s);
		}

		inline const T& operator[] (unsigned index) const
		{
			assert(index < 4);
			return data[index];
		}

		inline T& operator[] (unsigned index)
		{
			assert(index < 4);
			return data[index];
		}

		inline float length() const
		{
			return sqrt(x * x + y * y + z * z + w * w);
		}

		inline float dot(const vec4& b) const
		{
			return x * b.x + y * b.y + z * b.z + w * b.w;
		}

		inline vec4 normalize() const
		{
			auto l = length();
			return vec4(x / l, y / l, z / l, w / l);
		}

		inline vec4 lerp(const vec4& b, float t) const
		{
			return t < 0.0f ? *this : (t > 1.0f ? b : (*this * (1.0f - t) + b * t));;
		}

		inline void print() const
		{
			printf("[%f, %f, %f, %f]\n", x, y, z, w);
		}
	};

	using vec4f = vec4<float>;
	using vec4i = vec4<int32_t>;
	using vec4u = vec4<uint32_t>;

}
