#pragma once

#include <math/simd_float8.hpp>

namespace math
{
    namespace simd
    {
        struct vec4fx8
        {
            float8 x;
            float8 y;
            float8 z;
            float8 w;
            
            inline vec4fx8(const float8& _x = float8(), const float8& _y = float8(), const float8& _z = float8(), const float8& _w = float8()) : x(_x), y(_y), z(_z), w(_w) { }
            
            inline vec4fx8(const vec4fx8& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
            {
                
            }
            
            inline vec4fx8& operator=(const vec4fx8& rhs)
            {
                x = rhs.x;
                y = rhs.y;
                z = rhs.z;
                w = rhs.w;
                return *this;
            }
            
            inline friend vec4fx8 operator+(const vec4fx8& lhs, const vec4fx8& rhs)
            {
                return vec4fx8(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
            }
            
            inline friend vec4fx8 operator-(const vec4fx8& lhs, const vec4fx8& rhs)
            {
                return vec4fx8(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
            }
            
            inline float8 dot(const vec4fx8& rhs)
            {
                return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
            }
            
            inline vec4fx8 cross(const vec4fx8& rhs)
            {
                return vec4fx8(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
            }
        };
    }
}
