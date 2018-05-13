#pragma once

#include <immintrin.h>

namespace math
{
	namespace simd
	{
		struct float4
		{
            __m128 data;

			inline float4(const float& _x = 0.0f, const float& _y = 0.0f, const float& _z = 0.0f, const float& _w = 0.0f)
			{
				float v[] = { _x, _y, _z, _w };
				load(&v[0]);
			}

			inline float4(__m128 _data) : data(_data)
			{

			}

			inline float4(const float* _data)
			{
				load(_data);
			}
            
            inline float4(const float4& rhs) : data(rhs.data)
            {
                
            }
            
            inline float4& operator=(const __m128& rhs)
            {
                data = rhs;
                return *this;
            }
            
            inline float4& operator=(const float4& rhs)
            {
                data = rhs.data;
                return *this;
            }

			inline void load(const float* _data)
			{
				data = _mm_load_ps(_data);
			}

			inline void store(float* _data)
			{
				_mm_store_ps(_data, data);
			}

			friend float4 operator+(const float4& lhs, const float4& rhs)
			{
				return _mm_add_ps(lhs.data, rhs.data);
			}

			friend float4 operator-(const float4& lhs, const float4& rhs)
			{
				return _mm_sub_ps(lhs.data, rhs.data);
			}

			friend float4 operator*(const float4& lhs, const float4& rhs)
			{
				return _mm_mul_ps(lhs.data, rhs.data);
			}

			friend float4 operator/(const float4& lhs, const float4& rhs)
			{
				return _mm_div_ps(lhs.data, rhs.data);
			}
		};
	}
}
