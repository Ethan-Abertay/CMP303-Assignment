#pragma once
#include "DXF.h"	// include dxframework

// It's very annoying that XMFLOAT3 only has the Operator =

class XM_Math
{
public:

	static XMFLOAT3 subtract(XMFLOAT3 a, XMFLOAT3 b)
	{
		a.x = a.x - b.x;
		a.y = a.y - b.y;
		a.z = a.z - b.z;
		return a;
	}

	static XMFLOAT3 subtract(XMFLOAT3 f3, float f)
	{
		f3.x = f3.x - f;
		f3.y = f3.y - f;
		f3.z = f3.z - f;
		return f3;
	}

	static void add(XMFLOAT3& a, XMFLOAT3 b)
	{
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
	}

	static XMFLOAT3 add(XMFLOAT3 a, XMFLOAT3 b)
	{
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	static XMFLOAT3 add(XMFLOAT3 f3, float f)
	{
		XMFLOAT3 output;
		output.x = f3.x + f;
		output.y = f3.y + f;
		output.z = f3.z + f;
		return output;
	}

	static float sum(XMFLOAT3 f)
	{
		return f.x + f.y + f.z;
	}

	static XMFLOAT3 multiply(XMFLOAT3 f3, float f)
	{
		XMFLOAT3 output;
		output.x = f3.x * f;
		output.y = f3.y * f;
		output.z = f3.z * f;
		return output;
	}

	static float magnitude(XMFLOAT3 f)
	{
		return sqrtf(f.x * f.x + f.y * f.y + f.z * f.z);
	}

	static float magnitude_squared(XMFLOAT3 f)
	{
		return (f.x * f.x + f.y * f.y + f.z * f.z);
	}

	static float scalar(XMFLOAT3 a, XMFLOAT3 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	static XMFLOAT3 cross(XMFLOAT3 a, XMFLOAT3 b)
	{
		XMFLOAT3 output;
		output.x = (a.y * b.z) - (a.z * b.y);
		output.y = (a.z * b.x) - (a.x * b.z);
		output.z = (a.x * b.y) - (a.y * b.x);
		return output;
	}

	static float angleBetween(XMFLOAT3 a, XMFLOAT3 b)
	{
		// Degrees good, Radians bad
		return XMConvertToDegrees(acosf((scalar(a, b))/(magnitude(a) * magnitude(b))));
	}

	static XMFLOAT3 inverse(XMFLOAT3 f)
	{
		f.x = -f.x;
		f.y = -f.y;
		f.z = -f.z;
		return f;
	}

	static XMFLOAT3 absolute(XMFLOAT3 f)
	{
		XMFLOAT3 output;
		output.x = fabsf(f.x);
		output.y = fabsf(f.y);
		output.z = fabsf(f.z);
		return output;
	}

	static float linePointDistance(XMFLOAT3 line, XMFLOAT3 point)
	{
		// Line can represent a known point on the line
		XMFLOAT3 hypotenuse = subtract(point, line);
		float angle = angleBetween(line, hypotenuse);
		return magnitude(hypotenuse) * sinf(XMConvertToRadians(angle));
	}

	static XMFLOAT3 normalise(XMFLOAT3 f)
	{
		XMFLOAT3 output = f;
		float sum = sqrtf(f.x * f.x + f.y * f.y + f.z * f.z);
		output.x /= sum;
		output.y /= sum;
		output.z /= sum;
		return output;
	}

	static XMFLOAT3 squared(XMFLOAT3 f)
	{
		XMFLOAT3 output;
		output.x = f.x * f.x;
		output.y = f.y * f.y;
		output.z = f.z * f.z;
		return output;
	}

	static float distance_squared(XMFLOAT3 a, XMFLOAT3 b)
	{
		return sum(squared(subtract(a, b)));
	}
};