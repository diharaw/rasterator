#pragma once

#include <math/vec3.hpp>

namespace math
{

	template<typename T>
	struct mat3
	{
		union
		{
			struct
			{
				// m[row][column]
				T m11, m21, m31, m12, m22, m32, m13, m23, m33;
			};
			float	elem[9];
			vec3<T> column[3];
		};

		mat3()
		{
			for (int i = 0; i < 9; i++)
				elem[i] = 0;

			m11 = 1;
			m22 = 1;
			m33 = 1;
		}

		mat3(float _m11, float _m12, float _m13,
			float _m21, float _m22, float _m23,
			float _m31, float _m32, float _m33)
		{
			m11 = _m11;
			m12 = _m12;
			m13 = _m13;

			m21 = _m21;
			m22 = _m22;
			m23 = _m23;

			m31 = _m31;
			m32 = _m32;
			m33 = _m33;
		}

		mat3(vec3<T> _c1, vec3<T> _c2, vec3<T> _c3)
		{
			m11 = _c1.x;
			m21 = _c1.y;
			m31 = _c1.z;

			m12 = _c2.x;
			m22 = _c2.y;
			m32 = _c2.z;

			m13 = _c3.x;
			m23 = _c3.y;
			m33 = _c3.z;
		}

		inline mat3 transpose() const
		{
			mat3 r;

			r.m11 = m11;
			r.m21 = m12;
			r.m31 = m13;

			r.m12 = m21;
			r.m22 = m22;
			r.m32 = m23;

			r.m13 = m31;
			r.m23 = m32;
			r.m33 = m33;

			return r;
		}

		inline T determinant() const
		{
			return (m11 * ((m22 * m33) - (m23 * m32))) - (m12 * ((m21 * m33) - (m23 * m31))) - (m13 * ((m21 * m32) - (m22 * m31)));
		}

		inline mat3 adjoint() const
		{
			mat3 r;

			r.m11 =   (m22 * m33) - (m23 * m32);
			r.m12 = -((m21 * m33) - (m23 * m31));
			r.m13 =   (m21 * m32) - (m22 * m31);

			r.m21 = -((m12 * m33) - (m13 * m32));
			r.m22 =   (m11 * m33) - (m13 * m31);
			r.m23 = -((m11 * m32) - (m12 * m31));

			r.m31 =   (m12 * m23) - (m13 * m22);
			r.m32 = -((m11 * m23) - (m13 * m21));
			r.m33 =   (m11 * m22) - (m12 * m21);

			return r.transpose();
		}

		inline mat3 inverse() const
		{
			return adjoint() / determinant(); // @NOTE(Dihara): Watch out for divide by zero!
		}

		inline void print() const
		{
			printf("[%f, %f, %f]\n", m11, m12, m13);
			printf("[%f, %f, %f]\n", m21, m22, m23);
			printf("[%f, %f, %f]\n", m31, m32, m33);
		}

		inline const vec3<T>& operator[] (unsigned index) const
		{
			assert(index < 3);
			return column[index];
		}

		inline vec3<T>& operator[] (unsigned index)
		{
			assert(index < 3);
			return column[index];
		}

		friend mat3 operator*(const mat3& lhs, const T& rhs)
		{
			mat3 r;

			for (int i = 0; i < 9; i++)
				r.elem[i] = lhs.elem[i] * rhs;

			return r;
		}

		friend vec3<T> operator*(const mat3& lhs, const vec3<T>& rhs)
		{
			vec3<T> r;

			r.x = (lhs.m11 * rhs.x) + (lhs.m12 * rhs.y) + (lhs.m13 * rhs.z);
			r.y = (lhs.m21 * rhs.x) + (lhs.m22 * rhs.y) + (lhs.m23 * rhs.z);
			r.z = (lhs.m31 * rhs.x) + (lhs.m32 * rhs.y) + (lhs.m33 * rhs.z);

			return r;
		}

		friend mat3 operator*(const mat3& lhs, const mat3& rhs)
		{
			mat3 r;

			r.m11 = (lhs.m11 * rhs.m11) + (lhs.m12 * rhs.m21) + (lhs.m13 * rhs.m31);
			r.m12 = (lhs.m11 * rhs.m12) + (lhs.m12 * rhs.m22) + (lhs.m13 * rhs.m32);
			r.m13 = (lhs.m11 * rhs.m13) + (lhs.m12 * rhs.m23) + (lhs.m13 * rhs.m33);

			r.m21 = (lhs.m21 * rhs.m11) + (lhs.m22 * rhs.m21) + (lhs.m23 * rhs.m31);
			r.m22 = (lhs.m21 * rhs.m12) + (lhs.m22 * rhs.m22) + (lhs.m23 * rhs.m32);
			r.m23 = (lhs.m21 * rhs.m13) + (lhs.m22 * rhs.m23) + (lhs.m23 * rhs.m33);

			r.m31 = (lhs.m31 * rhs.m11) + (lhs.m32 * rhs.m21) + (lhs.m33 * rhs.m31);
			r.m32 = (lhs.m31 * rhs.m12) + (lhs.m32 * rhs.m22) + (lhs.m33 * rhs.m32);
			r.m33 = (lhs.m31 * rhs.m13) + (lhs.m32 * rhs.m23) + (lhs.m33 * rhs.m33);

			return r;
		}
	};

	using mat3f = mat3<float>;

}