#pragma once

#include <math.h>
#include <math/vec3.hpp>

namespace math
{
	struct quat
	{
		float w;
		vec3f v;

		quat() : w(1.0f), v(0.0f, 0.0f, 0.0f) { }

		quat(const float& _w, const float& _x, const float& _y, const float& _z) : w(_w), v(_x, _y, _z) { }

		quat(const float& _radians, const vec3f& _axis)
		{
			w = cosf(_radians / 2.0f); // @NOTE(Dihara): Is the half-angle supposed to be in radians or degrees?
			v = _axis * sinf(_radians / 2.0f);
		}

		friend quat operator*(const quat& lhs, const quat& rhs)
		{
			quat q;

			q.w = (lhs.w * rhs.w) - (lhs.v.dot(rhs.v));
			q.v = rhs.v * lhs.w + lhs.v * rhs.w + (lhs.v.cross(rhs.v));

			return q;
		}

		friend quat operator-(const quat& lhs, const quat& rhs)
		{
			return rhs * lhs.inverse();
		}

		inline float magnitude() const
		{
			float v_mag = v.length();
			return sqrtf((w * w) + (v_mag * v_mag));
		}

		inline quat conjugate() const
		{
			return quat(w, -v.x, -v.y, -v.z);
		}

		inline quat inverse() const
		{
			// q^-1 = q* / ||q||

			quat q = conjugate();
			float m = magnitude();

			q.w /= m;
			q.v = q.v / m;

			return  q; // @NOTE(Dihara): Beware of divide-by-zero!
		}

		inline float dot(const quat& rhs) const
		{
			return (w * rhs.w) + (v.x * rhs.v.x) + (v.y * rhs.v.y) + (v.z * rhs.v.z);
		}

		inline quat exp(const float& e) const
		{
			quat q;

			if (fabs(w) < 0.9999f)
			{
				float alpha = acos(w);
				float new_alpha = alpha * e;

				q.w = cos(new_alpha);

				float mult = sin(new_alpha) / sin(alpha);
				q.v = v * mult;
			}

			return q;
		}

		inline quat slerp(const quat& q2, const float& t) const
		{
			quat q1 = *this;
			quat d = q1 - q2;
			quat d_exp = d.exp(t);
			return d_exp * q1;
		}
	};
}