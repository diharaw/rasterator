#pragma once

#include <math/vec4.hpp>
#include <assert.h>

namespace math
{

	template<typename T>
	struct mat4
	{
		union
		{
			struct
			{
				// m[row][column]
				T m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44;
			};
			float	elem[16];
			vec4<T> column[4];
		};

		mat4()
		{
			for (int i = 0; i < 16; i++)
				elem[i] = 0;

			m11 = 1;
			m22 = 1;
			m33 = 1;
			m44 = 1;
		}

		mat4(const float& _m11, const float& _m12, const float& _m13, const float& _m14,
			 const float& _m21, const float& _m22, const float& _m23, const float& _m24,
			 const float& _m31, const float& _m32, const float& _m33, const float& _m34,
			 const float& _m41, const float& _m42, const float& _m43, const float& _m44)
		{
			m11 = _m11;
			m12 = _m12;
			m13 = _m13;
			m14 = _m14;

			m21 = _m21;
			m22 = _m22;
			m23 = _m23;
			m24 = _m24;

			m31 = _m31;
			m32 = _m32;
			m33 = _m33;
			m34 = _m34;

			m41 = _m41;
			m42 = _m42;
			m43 = _m43;
			m44 = _m44;
		}

		mat4(const vec4<T>& _c1, const vec4<T>& _c2, const vec4<T>& _c3, const vec4<T>& _c4)
		{
			m11 = _c1.x;
			m21 = _c1.y;
			m31 = _c1.z;
			m41 = _c1.w;

			m12 = _c2.x;
			m22 = _c2.y;
			m32 = _c2.z;
			m42 = _c2.w;

			m13 = _c3.x;
			m23 = _c3.y;
			m33 = _c3.z;
			m43 = _c3.w;

			m14 = _c4.x;
			m24 = _c4.y;
			m34 = _c4.z;
			m44 = _c4.w;
		}

		inline mat4 transpose() const
		{
			mat4 r;

			r.m11 = m11;
			r.m21 = m12;
			r.m31 = m13;
			r.m41 = m14;

			r.m12 = m21;
			r.m22 = m22;
			r.m32 = m23;
			r.m42 = m24;

			r.m13 = m31;
			r.m23 = m32;
			r.m33 = m33;
			r.m43 = m34;

			r.m14 = m41;
			r.m24 = m42;
			r.m34 = m43;
			r.m44 = m44;

			return r;
		}

		inline T determinant() const
		{
			return (m11 * (m22 * ((m33 * m44) - (m34 * m43)) + m23 * ((m34 * m42) - (m32 * m44)) + m24 * ((m32 * m43) - (m33 * m42))))
				 - (m12 * (m21 * ((m33 * m44) - (m34 * m43)) + m23 * ((m34 * m41) - (m31 * m44)) + m24 * ((m31 * m43) - (m33 * m41))))
				 + (m13 * (m21 * ((m32 * m44) - (m34 * m42)) + m22 * ((m34 * m41) - (m31 * m44)) + m24 * ((m31 * m42) - (m32 * m41))))
				 - (m14 * (m21 * ((m32 * m43) - (m33 * m42)) + m22 * ((m33 * m41) - (m31 * m43)) + m23 * ((m31 * m42) - (m32 * m41))));
		}

		inline mat4 adjoint() const
		{
			mat4 r;

			r.m11 =   (m22 * m33 * m44) + (m23 * m34 * m42) + (m24 * m32 * m43) - (m22 * m34 * m43) - (m23 * m32 * m44) - (m24 * m33 * m42);
			r.m12 = -((m21 * m33 * m44) + (m23 * m34 * m41) + (m24 * m31 * m42) - (m21 * m34 * m43) - (m23 * m31 * m44) - (m24 * m33 * m41));
			r.m13 =   (m21 * m32 * m44) + (m22 * m34 * m42) + (m24 * m32 * m43) - (m21 * m34 * m42) - (m22 * m31 * m44) - (m24 * m32 * m41);
			r.m14 = -((m21 * m32 * m43) + (m22 * m33 * m41) + (m23 * m31 * m42) - (m21 * m33 * m42) - (m22 * m31 * m43) - (m23 * m32 * m41));

			r.m21 = -((m12 * m33 * m44) + (m13 * m34 * m42) + (m14 * m32 * m43) - (m12 * m34 * m43) - (m13 * m32 * m44) - (m14 * m33 * m42));
			r.m22 =   (m11 * m33 * m44) + (m13 * m34 * m41) + (m14 * m31 * m43) - (m11 * m34 * m43) - (m13 * m31 * m44) - (m14 * m33 * m41);
			r.m23 = -((m11 * m32 * m44) + (m12 * m34 * m41) + (m14 * m31 * m42) - (m11 * m34 * m42) - (m12 * m31 * m44) - (m14 * m32 * m41));
			r.m24 =   (m11 * m32 * m43) + (m12 * m33 * m41) + (m13 * m31 * m42) - (m11 * m33 * m42) - (m12 * m31 * m43) - (m13 * m32 * m41);

			r.m31 =   (m12 * m23 * m44) + (m13 * m24 * m42) + (m14 * m22 * m43) - (m12 * m24 * m43) - (m13 * m22 * m44) - (m14 * m23 * m42);
			r.m32 = -((m11 * m23 * m44) + (m13 * m24 * m41) + (m14 * m21 * m43) - (m11 * m24 * m43) - (m13 * m21 * m44) - (m14 * m23 * m41));
			r.m33 =   (m11 * m22 * m44) + (m12 * m24 * m41) + (m14 * m21 * m42) - (m11 * m24 * m42) - (m12 * m21 * m44) - (m14 * m22 * m41);
			r.m34 = -((m11 * m22 * m43) + (m12 * m23 * m41) + (m13 * m21 * m42) - (m11 * m23 * m42) - (m12 * m21 * m43) - (m13 * m22 * m41));

			r.m41 = -((m12 * m23 * m34) + (m13 * m24 * m32) + (m14 * m22 * m33) - (m12 * m24 * m33) - (m13 * m22 * m34) - (m14 * m23 * m32));
			r.m42 =   (m11 * m23 * m34) + (m13 * m24 * m31) + (m14 * m21 * m33) - (m11 * m24 * m33) - (m13 * m21 * m34) - (m14 * m23 * m31);
			r.m43 = -((m11 * m22 * m34) + (m12 * m24 * m31) + (m14 * m21 * m32) - (m11 * m24 * m32) - (m12 * m21 * m34) - (m14 * m22 * m31));
			r.m44 =   (m11 * m22 * m33) + (m12 * m23 * m31) + (m13 * m21 * m32) - (m11 * m23 * m32) - (m12 * m21 * m33) - (m13 * m22 * m31);

			return r.transpose();
		}
 			 
		inline mat4 inverse() const
		{
			return adjoint() / determinant(); // @NOTE(Dihara): Watch out for divide by zero!
		}

		inline void print() const
		{
			printf("[%f, %f, %f, %f]\n", m11, m12, m13, m14);
			printf("[%f, %f, %f, %f]\n", m21, m22, m23, m24);
			printf("[%f, %f, %f, %f]\n", m31, m32, m33, m34);
			printf("[%f, %f, %f, %f]\n", m41, m42, m43, m44);
		}

		inline const vec4<T>& operator[] (unsigned index) const
		{
			assert(index < 4);
			return column[index];
		}

		inline vec4<T>& operator[] (unsigned index)
		{
			assert(index < 4);
			return column[index];
		}

		friend mat4 operator*(const mat4& lhs, const T& rhs)
		{
			mat4 r;

			for (int i = 0; i < 16; i++)
				r.elem[i] = lhs.elem[i] * rhs;

			return r;
		}
        
        friend mat4 operator/(const mat4& lhs, const T& rhs)
        {
            mat4 r;
            
            for (int i = 0; i < 16; i++)
                r.elem[i] = lhs.elem[i] / rhs;
            
            return r;
        }

		friend vec4<T> operator*(const mat4& lhs, const vec4<T>& rhs)
		{
			vec4<T> r;

			r.x = (lhs.m11 * rhs.x) + (lhs.m12 * rhs.y) + (lhs.m13 * rhs.z) + (lhs.m14 * rhs.w);
			r.y = (lhs.m21 * rhs.x) + (lhs.m22 * rhs.y) + (lhs.m23 * rhs.z) + (lhs.m24 * rhs.w);
			r.z = (lhs.m31 * rhs.x) + (lhs.m32 * rhs.y) + (lhs.m33 * rhs.z) + (lhs.m34 * rhs.w);
			r.w = (lhs.m41 * rhs.x) + (lhs.m42 * rhs.y) + (lhs.m43 * rhs.z) + (lhs.m44 * rhs.w);

			return r;
		}

		friend mat4 operator*(const mat4& lhs, const mat4& rhs)
		{
			mat4 r;

			r.m11 = (lhs.m11 * rhs.m11) + (lhs.m12 * rhs.m21) + (lhs.m13 * rhs.m31) + (lhs.m14 * rhs.m41);
			r.m12 = (lhs.m11 * rhs.m12) + (lhs.m12 * rhs.m22) + (lhs.m13 * rhs.m32) + (lhs.m14 * rhs.m42);
			r.m13 = (lhs.m11 * rhs.m13) + (lhs.m12 * rhs.m23) + (lhs.m13 * rhs.m33) + (lhs.m14 * rhs.m43);
			r.m14 = (lhs.m11 * rhs.m14) + (lhs.m12 * rhs.m24) + (lhs.m13 * rhs.m34) + (lhs.m14 * rhs.m44);

			r.m21 = (lhs.m21 * rhs.m11) + (lhs.m22 * rhs.m21) + (lhs.m23 * rhs.m31) + (lhs.m24 * rhs.m41);
			r.m22 = (lhs.m21 * rhs.m12) + (lhs.m22 * rhs.m22) + (lhs.m23 * rhs.m32) + (lhs.m24 * rhs.m42);
			r.m23 = (lhs.m21 * rhs.m13) + (lhs.m22 * rhs.m23) + (lhs.m23 * rhs.m33) + (lhs.m24 * rhs.m43);
			r.m24 = (lhs.m21 * rhs.m14) + (lhs.m22 * rhs.m24) + (lhs.m23 * rhs.m34) + (lhs.m24 * rhs.m44);

			r.m31 = (lhs.m31 * rhs.m11) + (lhs.m32 * rhs.m21) + (lhs.m33 * rhs.m31) + (lhs.m34 * rhs.m41);
			r.m32 = (lhs.m31 * rhs.m12) + (lhs.m32 * rhs.m22) + (lhs.m33 * rhs.m32) + (lhs.m34 * rhs.m42);
			r.m33 = (lhs.m31 * rhs.m13) + (lhs.m32 * rhs.m23) + (lhs.m33 * rhs.m33) + (lhs.m34 * rhs.m43);
			r.m34 = (lhs.m31 * rhs.m14) + (lhs.m32 * rhs.m24) + (lhs.m33 * rhs.m34) + (lhs.m34 * rhs.m44);

			r.m41 = (lhs.m41 * rhs.m11) + (lhs.m42 * rhs.m21) + (lhs.m43 * rhs.m31) + (lhs.m44 * rhs.m41);
			r.m42 = (lhs.m41 * rhs.m12) + (lhs.m42 * rhs.m22) + (lhs.m43 * rhs.m32) + (lhs.m44 * rhs.m42);
			r.m43 = (lhs.m41 * rhs.m13) + (lhs.m42 * rhs.m23) + (lhs.m43 * rhs.m33) + (lhs.m44 * rhs.m43);
			r.m44 = (lhs.m41 * rhs.m14) + (lhs.m42 * rhs.m24) + (lhs.m43 * rhs.m34) + (lhs.m44 * rhs.m44);

			return r;
		}
	};

	

	using mat4f = mat4<float>;

}
