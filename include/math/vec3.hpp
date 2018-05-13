#pragma once

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

namespace math
{

	template<typename T>
	struct vec3
	{
		union
		{
			struct
			{
				T x, y, z;
			};
			T data[3];
		};

		vec3() : x(0.0), y(0.0), z(0.0) {}
		vec3(const T& v) : x(v), y(v), z(v) {}
		vec3(const T& _x, const T& _y, const T& _z) : x(_x), y(_y), z(_z) {}

		friend vec3<T> operator+(const vec3<T>& lhs, const vec3<T>& rhs) 
		{
			return vec3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
		}

		friend vec3<T> operator-(const vec3<T>& lhs, const vec3<T>& rhs) 
		{
			return vec3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
		}

		friend vec3<T> operator*(const vec3<T>& lhs, const T& rhs) 
		{
			return vec3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
		}

		friend vec3<T> operator*(const T& lhs, const vec3<T>& rhs) 
		{
			return vec3<T>(lhs * rhs.x, lhs * rhs.y, lhs* rhs.z);
		}

		friend vec3<T> operator/(const vec3<T>& lhs, const T& rhs) 
		{
			return vec3<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
		}

		inline const T& operator[] (unsigned index) const
		{
			assert(index < 3);
			return data[index];
		}

		inline T& operator[] (unsigned index)
		{
			assert(index < 3);
			return data[index];
		}

		inline float length() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		inline float distance(const vec3& v) const
		{
			auto r = *this - v;
			return r.length();
		}

		inline float dot(const vec3& b) const
		{
			return x * b.x + y * b.y + z * b.z;
		}

		inline vec3 cross(const vec3& b) const
		{
			return vec3(y * b.z - z * b.y,
				z * b.x - x * b.z,
				x * b.y - y * b.x);
		}

		inline vec3 normalize() const
		{
			auto l = length();
			return vec3(x / l, y / l, z / l);
		}

		inline vec3 direction(const vec3& to) const
		{
			auto v = to - *this;
			return v.normalize();
		}

		inline vec3 lerp(const vec3& b, float t) const
		{
			return t < 0.0f ? *this : (t > 1.0f ? b : (*this * (1.0f - t) + b * t));;
		}

		inline void print() const
		{
			printf("[%f, %f, %f]\n", x, y, z);
		}
	};

	using vec3f = vec3<float>;
	using vec3i = vec3<int32_t>;
	using vec3u = vec3<uint32_t>;

}