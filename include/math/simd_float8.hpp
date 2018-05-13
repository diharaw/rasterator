#pragma once

#include <immintrin.h>

namespace math
{
    namespace simd
    {
        struct float8
        {
            __m256 data;
            
            inline static void zero_upper()
            {
                _mm256_zeroupper();
            }
            
            inline float8(const float& _v0 = 0.0f, const float& _v1 = 0.0f, const float& _v2 = 0.0f, const float& _v3 = 0.0f, const float& _v4 = 0.0f, const float& _v5 = 0.0f, const float& _v6 = 0.0f, const float& _v7 = 0.0f)
            {
                float v[] = { _v0, _v1, _v2, _v3, _v4, _v5, _v6, _v7 };
                load(&v[0]);
            }
            
            inline float8(__m256 _data) : data(_data)
            {
                
            }
            
            inline float8(const float* _data)
            {
                load(_data);
            }
            
            inline float8(const float8& rhs) : data(rhs.data)
            {
                
            }
            
            inline float8& operator=(const __m256& rhs)
            {
                data = rhs;
                return *this;
            }
            
            inline float8& operator=(const float8& rhs)
            {
                data = rhs.data;
                return *this;
            }
            
            inline void load(const float* _data)
            {
                data = _mm256_load_ps(_data);
            }
            
            inline void store(float* _data)
            {
                _mm256_store_ps(_data, data);
            }
            
            friend float8 operator+(const float8& lhs, const float8& rhs)
            {
                return _mm256_add_ps(lhs.data, rhs.data);
            }
            
            friend float8 operator-(const float8& lhs, const float8& rhs)
            {
                return _mm256_sub_ps(lhs.data, rhs.data);
            }
            
            friend float8 operator*(const float8& lhs, const float8& rhs)
            {
                return _mm256_mul_ps(lhs.data, rhs.data);
            }
            
            friend float8 operator/(const float8& lhs, const float8& rhs)
            {
                return _mm256_div_ps(lhs.data, rhs.data);
            }
        };
    }
}
