#pragma once
#include<DirectXMath.h>
#include<vector>

#define EPS 1e-10
namespace Quaternion
{
	DirectX::XMFLOAT4 Get(DirectX::XMFLOAT3 axis, float angle);
	DirectX::XMFLOAT3 RotateByQuaternion(const DirectX::XMFLOAT3& v,
		const DirectX::XMFLOAT4& q);

	DirectX::XMFLOAT3 ToEulerAngles(DirectX::XMFLOAT4 q);
}

struct AARect
{
	DirectX::XMFLOAT2 a, b;
};


DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& l,
	const DirectX::XMFLOAT3& r);
DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& l,
	const DirectX::XMFLOAT3& r);
DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& l);

DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& l,
	const DirectX::XMFLOAT2& r);
DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& l,
	const DirectX::XMFLOAT2& r);
DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& l);

namespace vecmath
{
	float screenRay(DirectX::XMVECTOR point, DirectX::XMVECTOR ray,
		DirectX::XMMATRIX viewproj, float radius, float aspect);
	bool screenBox(DirectX::XMVECTOR point,
		DirectX::XMVECTOR lo, DirectX::XMVECTOR hi,
		DirectX::XMMATRIX viewproj);
	float clamp01(float v);
	float length(DirectX::XMFLOAT3 v);
	float lengthSq(DirectX::XMFLOAT3 v);
	float length(DirectX::XMFLOAT2 v);
	float lengthSq(DirectX::XMFLOAT2 v);
	float triArea(DirectX::XMFLOAT2 a, DirectX::XMFLOAT2 b, DirectX::XMFLOAT2 c);
	bool bb2d(DirectX::XMFLOAT2 a1, DirectX::XMFLOAT2 a2,
		DirectX::XMFLOAT2 b1, DirectX::XMFLOAT2 b2);
	bool bb2d(DirectX::XMFLOAT2 a1, DirectX::XMFLOAT2 a2,
		DirectX::XMFLOAT2 b);
	int segments2d(DirectX::XMFLOAT2 a1, DirectX::XMFLOAT2 a2,
		DirectX::XMFLOAT2 b1, DirectX::XMFLOAT2 b2);
	int segments2d(DirectX::XMFLOAT2 a1, DirectX::XMFLOAT2 a2,
		DirectX::XMFLOAT2 b1, DirectX::XMFLOAT2 b2, float& t, float& u);
	DirectX::XMFLOAT3 argwiseMul(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);
	DirectX::XMFLOAT3& get(DirectX::XMFLOAT3X3& m, int k);
}


DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& l,
	float r);
inline DirectX::XMFLOAT3 operator*(float l,
	const DirectX::XMFLOAT3& r) {
	return r * l;
}
DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& l,
	float r);
inline DirectX::XMFLOAT2 operator*(float l,
	const DirectX::XMFLOAT2& r) {
	return r * l;
}
#include "vecmath_internals.h"

__VECEQNEQ2DECL(DirectX::XMINT2)
__VECEQNEQ2DECL(DirectX::XMFLOAT2)