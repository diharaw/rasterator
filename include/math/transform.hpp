#pragma once

#include <math/vec3.hpp>
#include <math/mat4.hpp>
#include <math/quat.hpp>

#define TE_RIGHT_HANDED

namespace math
{
	inline mat4f lookat_lh(const vec3f& _eye, const vec3f& _origin, const vec3f& _up)
	{
		vec3f front = _eye.direction(_origin);
		vec3f right = _up.cross(front).normalize();
		vec3f up = front.cross(right).normalize();

		mat4f m;

		m.m11 = right.x;
		m.m12 = right.y;
		m.m13 = right.z;

		m.m21 = up.x;
		m.m22 = up.y;
		m.m23 = up.z;

		m.m31 = front.x;
		m.m32 = front.y;
		m.m33 = front.z;

		m.m14 = -right.dot(_eye);
		m.m24 = -up.dot(_eye);
		m.m34 = -front.dot(_eye);

		return m;
	}

	inline mat4f lookat_rh(const vec3f& _eye, const vec3f& _origin, const vec3f& _up)
	{
		vec3f front = (_origin - _eye).normalize();
		vec3f right = front.cross(_up).normalize();
		vec3f up = right.cross(front).normalize();

		mat4f m;

		m.m11 = right.x;
		m.m12 = right.y;
		m.m13 = right.z;

		m.m21 = up.x;
		m.m22 = up.y;
		m.m23 = up.z;

		m.m31 = -front.x;
		m.m32 = -front.y;
		m.m33 = -front.z;

		m.m14 = -right.dot(_eye);
		m.m24 = -up.dot(_eye);
		m.m34 = front.dot(_eye);

		return m;
	}

	inline mat4f lookat(const vec3f& eye, const vec3f& origin, const vec3f& up)
	{
#	if defined(TE_RIGHT_HANDED)
		return lookat_rh(eye, origin, up);
#	else
		return lookat_lh(eye, origin, up);
#	endif
	}

	inline mat4f perspective_lh_zo(const float& _aspect, const float& _fov, const float& _near, const float& _far)
	{
		mat4f m;

		float tanFoV2 = tanf(_fov / 2.0f);

		m.m11 = 1.0f / (_aspect * tanFoV2);
		m.m22 = 1.0f / tanFoV2;
		m.m33 = _far / (_far - _near);
		m.m34 = -(_far * _near) / (_far - _near);
		m.m43 = 1.0f;
		m.m44 = 0.0f;

		return m;
	}

	inline mat4f perspective_lh_no(const float& _aspect, const float& _fov, const float& _near, const float& _far)
	{
		mat4f m;

		float tanFoV2 = tanf(_fov / 2.0f);

		m.m11 = 1.0f / (_aspect * tanFoV2);
		m.m22 = 1.0f / tanFoV2;
		m.m33 = (_far + _near) / (_far - _near);
		m.m34 = -(2.0f * _far * _near) / (_far - _near);
		m.m43 = 1.0f;
		m.m44 = 0.0f;

		return m;
	}

	inline mat4f perspective_rh_zo(const float& _aspect, const float& _fov, const float& _near, const float& _far)
	{
		mat4f m;

		float tanFoV2 = tanf(_fov / 2.0f);

		m.m11 = 1.0f / (_aspect * tanFoV2);
		m.m22 = 1.0f / tanFoV2;
		m.m33 = _far / (_near - _far);
		m.m34 = -(_far * _near) / (_far - _near);
		m.m43 = -1.0f;
		m.m44 = 0.0f;

		return m;
	}

	inline mat4f perspective_rh_no(const float& _aspect, const float& _fov, const float& _near, const float& _far)
	{
		mat4f m;

		float tanFoV2 = tanf(_fov / 2.0f);

		m.m11 = 1.0f / (_aspect * tanFoV2);
		m.m22 = 1.0f / tanFoV2;
		m.m33 = -(_far + _near) / (_far - _near);
		m.m34 = -(2.0f * _far * _near) / (_far - _near);
		m.m43 = -1.0f;
		m.m44 = 0.0f;

		return m;
	}

	inline mat4f perspective(const float& _aspect, const float& _fov, const float& _near, const float& _far)
	{
#if defined(TE_RIGHT_HANDED)
#	if defined(TE_ZERO_TO_ONE)
		return perspective_rh_zo(_aspect, _fov, _near, _far);
#	else
		return perspective_rh_no(_aspect, _fov, _near, _far);
#	endif
#else
#	if defined(TE_ZERO_TO_ONE)
		return perspective_lh_zo(_aspect, _fov, _near, _far);
#	else
		return perspective_lh_no(_aspect, _fov, _near, _far);
#	endif
#endif
	}

	inline mat4f ortho_lh_zo(const float& _l, const float& _r, const float& _b, const float& _t, const float& _n, const float& _f)
	{
		mat4f m;

		m.m11 = 2.0f / (_r - _l);
		m.m22 = 2.0f / (_t - _b);
		m.m33 = 1.0f / (_f - _n);
		m.m14 = -(_r + _l) / (_r - _l);
		m.m24 = -(_t + _b) / (_t - _b);
		m.m34 = -_n / (_f - _n);

		return m;
	}

	inline mat4f ortho_lh_no(const float& _l, const float& _r, const float& _b, const float& _t, const float& _n, const float& _f)
	{
		mat4f m;

		m.m11 = 2.0f / (_r - _l);
		m.m22 = 2.0f / (_t - _b);
		m.m33 = 2.0f / (_f - _n);
		m.m14 = -(_r + _l) / (_r - _l);
		m.m24 = -(_t + _b) / (_t - _b);
		m.m34 = -(_f + _n) / (_f - _n);

		return m;
	}

	inline mat4f ortho_rh_zo(const float& _l, const float& _r, const float& _b, const float& _t, const float& _n, const float& _f)
	{
		mat4f m;

		m.m11 = 2.0f / (_r - _l);
		m.m22 = 2.0f / (_t - _b);
		m.m33 = -1.0f / (_f - _n);
		m.m14 = -(_r + _l) / (_r - _l);
		m.m24 = -(_t + _b) / (_t - _b);
		m.m34 = -_n / (_f - _n);

		return m;
	}

	inline mat4f ortho_rh_no(const float& _l, const float& _r, const float& _b, const float& _t, const float& _n, const float& _f)
	{
		mat4f m;

		m.m11 = 2.0f / (_r - _l);
		m.m22 = 2.0f / (_t - _b);
		m.m33 = -2.0f / (_f - _n);
		m.m14 = -(_r + _l) / (_r - _l);
		m.m24 = -(_t + _b) / (_t - _b);
		m.m34 = -(_f + _n) / (_f - _n);

		return m;
	}

	inline mat4f ortho(const float& _l, const float& _r, const float& _b, const float& _t, const float& _n, const float& _f)
	{
#if defined(TE_RIGHT_HANDED)
#	if defined(TE_ZERO_TO_ONE)
		return ortho_rh_zo(_l, _r, _b, _t, _n, _f);
#	else
		return ortho_rh_no(_l, _r, _b, _t, _n, _f);
#	endif
#else
#	if defined(TE_ZERO_TO_ONE)
		return ortho_lh_zo(_l, _r, _b, _t, _n, _f);
#	else
		return ortho_lh_no(_l, _r, _b, _t, _n, _f);
#	endif
#endif
	}

	inline mat4f rotation(const float& _radians, const vec3f& _axis)
	{
		mat4f m;

		float cosTheta = cosf(_radians);
		float sinTheta = sinf(_radians);

		m.m11 = powf(_axis.x, 2.0f) * (1.0f - cosTheta) + cosTheta;
		m.m12 = _axis.x * _axis.y * (1.0f - cosTheta) - _axis.z * sinTheta;
		m.m13 = _axis.x * _axis.z * (1.0f - cosTheta) + _axis.y * sinTheta;

		m.m21 = _axis.x * _axis.y * (1.0f - cosTheta) + _axis.z * sinTheta;
		m.m22 = powf(_axis.y, 2.0f) * (1.0f - cosTheta) + cosTheta; 
		m.m23 = _axis.y * _axis.z * (1.0f - cosf(_radians)) - _axis.x * sinTheta;

		m.m31 = _axis.x * _axis.z * (1.0f - cosTheta) - _axis.y * sinTheta;
		m.m32 = _axis.y * _axis.z * (1.0f - cosTheta) + _axis.x * sinTheta;
		m.m33 = powf(_axis.z, 2.0f) * (1.0f - cosTheta) + cosTheta;

		return m;
	}

	inline mat4f rotation(const float& _x, const float& _y, const float& _z)
	{
		mat4f B, P, H;

		P.m22 = cosf(_x);
		P.m23 = -sinf(_x);

		P.m32 = sinf(_x);
		P.m33 = cosf(_x);

		H.m11 = cosf(_y);
		H.m13 = sinf(_y);

		H.m31 = -sinf(_y);
		H.m33 = cosf(_y);

		B.m11 = cosf(_z);
		B.m12 = -sinf(_z);

		B.m21 = sinf(_z);
		B.m22 = cosf(_z);

		return H * P * B;
	}

	inline mat4f translation(const vec3f& _position)
	{
		mat4f m;

		m.m14 = _position.x;
		m.m24 = _position.y;
		m.m34 = _position.z;

		return m;
	}

	inline mat4f scale(const vec3f& _scale)
	{
		mat4f m;

		m.m11 = _scale.x;
		m.m22 = _scale.y;
		m.m33 = _scale.z;

		return m;
	}

	inline mat4f scale(const float& _scale, const vec3f& _axis)
	{
		mat4f m;

		m.m11 = 1.0f + (_scale - 1.0f) * powf(_axis.x, 2.0f);
		m.m12 = (_scale - 1.0f) * _axis.x * _axis.y;
		m.m13 = (_scale - 1.0f) * _axis.x * _axis.z;

		m.m21 = (_scale - 1.0f) * _axis.x * _axis.y;
		m.m22 = 1.0f + (_scale - 1.0f) * powf(_axis.y, 2.0f);
		m.m23 = (_scale - 1.0f) * _axis.y * _axis.z;

		m.m31 = (_scale - 1.0f) * _axis.x * _axis.z;
		m.m32 = (_scale - 1.0f) * _axis.y * _axis.z;
		m.m33 = 1.0f + (_scale - 1.0f) * powf(_axis.z, 2.0f);

		return m;
	}

	inline mat4f euler_to_mat4(const float& _x, const float& _y, const float& _z)
	{
		mat4f m;

		float ch = cosf(_y);
		float cb = cosf(_z);
		float cp = cosf(_x);
		float sh = sinf(_y);
		float sb = sinf(_z);
		float sp = sinf(_x);

		m.m11 = ch * cb + sh * sp * sb;
		m.m12 = ch * sb + sh * sp * cb;
		m.m13 = sh * cp;

		m.m21 = sb * cp;
		m.m22 = cb * cp;
		m.m23 = -sp;

		m.m31 = -sh * cb + ch * sp * sb;
		m.m32 = sb * sh + ch * sp * cb;
		m.m33 = ch * cp;

		return m;
	}

	inline mat4f quat_to_mat4(const quat& _quat)
	{
		mat4f m;

		m.m11 = 1.0f - (2.0f * (_quat.v.y * _quat.v.y)) - (2.0f * (_quat.v.z * _quat.v.z));
		m.m12 = (2.0f * (_quat.v.x * _quat.v.y)) - (2.0f * (_quat.w * _quat.v.z));
		m.m13 = (2.0f * (_quat.v.x * _quat.v.z)) + (2.0f * (_quat.w * _quat.v.y));

		m.m21 = (2.0f * (_quat.v.x * _quat.v.y)) + (2.0f * (_quat.w * _quat.v.z));
		m.m22 = 1.0f - (2.0f * (_quat.v.x * _quat.v.z)) - (2.0f * (_quat.v.z * _quat.v.z));
		m.m23 = (2.0f * (_quat.v.y * _quat.v.z)) + (2.0f * (_quat.w * _quat.v.x));

		m.m31 = (2.0f * (_quat.v.x * _quat.v.y)) - (2.0f * (_quat.w * _quat.v.z));
		m.m32 = (2.0f * (_quat.v.y * _quat.v.z)) + (2.0f * (_quat.w * _quat.v.x));
		m.m33 = 1.0f - (2.0f * (_quat.v.x * _quat.v.x)) - (2.0f * (_quat.v.y * _quat.v.y));

		return m;
	}

	inline quat mat4_to_quat(const mat4f& _mat)
	{
		quat q;

		float fourWSquaredMinus1 = _mat.m11 + _mat.m22 + _mat.m33;
		float fourXSquaredMinus1 = _mat.m11 - _mat.m22 - _mat.m33;
		float fourYSquaredMinus1 = _mat.m22 - _mat.m11 - _mat.m33;
		float fourZSquaredMinus1 = _mat.m33 - _mat.m11 - _mat.m22;

		int biggestIndex = 0;
		float fourBiggestSquaredMinus1 = fourWSquaredMinus1;

		if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}

		float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
		float mult = 0.25f / biggestVal;

		switch (biggestIndex)
		{
		case 0:
		{
			q.w = biggestVal;
			q.v.x = (_mat.m32 - _mat.m23) * mult;
			q.v.y = (_mat.m13 - _mat.m31) * mult;
			q.v.z = (_mat.m21 - _mat.m12) * mult;
			break;
		}
		case 1:
		{
			q.v.x = biggestVal;
			q.w = (_mat.m32 - _mat.m23) * mult;
			q.v.y = (_mat.m21 - _mat.m12) * mult;
			q.v.z = (_mat.m13 - _mat.m31) * mult;
			break;
		}
		case 2:
		{
			q.v.y = biggestVal;
			q.w = (_mat.m13 - _mat.m31) * mult;
			q.v.x = (_mat.m21 - _mat.m12) * mult;
			q.v.z = (_mat.m32 - _mat.m23) * mult;
			break;
		}
		case 3:
		{
			q.v.z = biggestVal;
			q.w = (_mat.m21 - _mat.m12) * mult;
			q.v.x = (_mat.m13 - _mat.m31) * mult;
			q.v.y = (_mat.m32 - _mat.m23) * mult;
			break;
		}
		}

		return q;
	}

	inline quat euler_to_quat(const float& _x, const float& _y, const float& _z)
	{
		quat q;

		q.w = cosf(_y / 2.0f) * cosf(_x / 2.0f) * cosf(_z / 2.0f) + sinf(_y / 2.0f) * sinf(_x / 2.0f) * sinf(_z / 2.0f);
		q.v.x = cosf(_y / 2.0f) * sinf(_x / 2.0f) * cosf(_z / 2.0f) - sinf(_y / 2.0f) * cosf(_x / 2.0f) * sinf(_z / 2.0f);
		q.v.y = cosf(_y / 2.0f) * sinf(_x / 2.0f) * sinf(_z / 2.0f) - sinf(_y / 2.0f) * cosf(_x / 2.0f) * cosf(_z / 2.0f);
		q.v.z = sinf(_y / 2.0f) * sinf(_x / 2.0f) * cosf(_z / 2.0f) - cosf(_y / 2.0f) * cosf(_x / 2.0f) * sinf(_z / 2.0f);

		return q;
	}

	inline vec3f mat4_to_euler(const mat4f& _mat)
	{
		vec3f euler;

		float sp = -_mat.m23;

		if (sp <= -1.0f)
		{
			euler.x = -1.570796f;
		}
		else if (sp >= 1.0f)
			euler.x = 1.570796f;
		else
			euler.x = asinf(sp);

		if (fabs(sp) > 0.9999f)
		{
			euler.z = 0.0f;
			euler.y = atan2(-_mat.m31, _mat.m11);
		}
		else
		{
			euler.y = atan2(_mat.m31, _mat.m11);
			euler.z = atan2(_mat.m21, _mat.m22);
		}

		return euler;
	}

	inline vec3f quat_to_euler(const quat& _quat)
	{
		vec3f e;

		float sp = -2.0f * (_quat.v.y - _quat.w * _quat.v.x);

		if (fabs(sp) > 0.9999f)
		{
			e.x = 1.570796f * sp;
			e.y = atan2(-_quat.v.x * _quat.v.z + _quat.w * _quat.v.y, 0.5f - _quat.v.y * _quat.v.y - _quat.v.z * _quat.v.z);
			e.z = 0.0f;
		}
		else
		{
			e.x = asin(sp);
			e.y = atan2(_quat.v.x * _quat.v.z + _quat.w * _quat.v.y, 0.5f - _quat.v.x * _quat.v.x - _quat.v.y * _quat.v.y);
			e.z = atan2(_quat.v.x * _quat.v.y + _quat.w * _quat.v.z, 0.5f - _quat.v.x * _quat.v.x - _quat.v.z * _quat.v.z);
		}

		return e;
	}
}
