#pragma once

#include <math/simd_float4.hpp>

namespace math
{
	namespace simd
	{
		struct vec4fx4
		{
			float4 x;
			float4 y;
			float4 z;
			float4 w;

			inline vec4fx4(const float4& _x = float4(), const float4& _y = float4(), const float4& _z = float4(), const float4& _w = float4()) : x(_x), y(_y), z(_z), w(_w) { }
            
            inline vec4fx4(const vec4fx4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
            {
                
            }
            
            inline vec4fx4& operator=(const vec4fx4& rhs)
            {
                x = rhs.x;
                y = rhs.y;
                z = rhs.z;
                w = rhs.w;
                return *this;
            }
			
			inline friend vec4fx4 operator+(const vec4fx4& lhs, const vec4fx4& rhs)
			{
				return vec4fx4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
			}

			inline friend vec4fx4 operator-(const vec4fx4& lhs, const vec4fx4& rhs)
			{
				return vec4fx4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
			}

			inline float4 dot(const vec4fx4& rhs)
			{
				return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
			}

			inline vec4fx4 cross(const vec4fx4& rhs)
			{
				return vec4fx4(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
			}
		};
	}
}
