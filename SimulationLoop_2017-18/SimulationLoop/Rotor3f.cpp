#include "Rotor3f.h"
#include <math.h>

Rotor3f::Rotor3f()
= default;

Rotor3f::~Rotor3f()
= default;

Bivector3::Bivector3(float b01, float b02, float b12)
	: b01(b01), b02(b02), b12(b12) {}

// Wedge product
inline Bivector3 Wedge(const Vector3f& u, const Vector3f& v)
{
	Bivector3 ret(
		u.GetX() * v.GetY() - u.GetY() * v.GetX(), // XY
		u.GetX() * v.GetZ() - u.GetZ() * v.GetX(), // XZ
		u.GetY() * v.GetZ() - u.GetZ() * v.GetY()  // YZ
	);
	return ret;
}

// default ctor
Rotor3::Rotor3(float a, float b01, float b02, float b12)
	: a(a), b01(b01), b02(b02), b12(b12) {}

// construct the rotor that rotates one vector to another
// uses the usual trick to get the half angle
Rotor3::Rotor3(const Vector3f& vFrom, const Vector3f& vTo)
{
	a = 1 + vTo.dot(vFrom);
	// the left side of the products have b a, not a b, so flip
	Bivector3 minusb = Wedge(vTo, vFrom);
	b01 = minusb.b01;
	b02 = minusb.b02;
	b12 = minusb.b12;
	normalize();
}

// angle+plane, plane must be normalized
Rotor3::Rotor3(float angleRadian, const Bivector3& bvPlane)
{
	float sina = sin(angleRadian / 2.f);
	a = cos(angleRadian / 2.f);
	// the left side of the products have b a, not a b
	b01 = -sina * bvPlane.b01;
	b02 = -sina * bvPlane.b02;
	b12 = -sina * bvPlane.b12;
}

// Rotor3-Rotor3 product
// non-optimized
inline Rotor3 Rotor3::operator*(const Rotor3& q) const
{
	const Rotor3& p = *this;
	Rotor3 r;

	// here we just expanded the geometric product rules



	r.a = p.a * q.a
		- p.b01 * q.b01 - p.b02 * q.b02 - p.b12 * q.b12;

	r.b01 = p.b01 * q.a + p.a   * q.b01
		+ p.b12 * q.b02 - p.b02 * q.b12;

	r.b02 = p.b02 * q.a + p.a   * q.b02
		- p.b12 * q.b01 + p.b01 * q.b12;

	r.b12 = p.b12 * q.a + p.a   * q.b12
		+ p.b02 * q.b01 - p.b01 * q.b02;

	return r;
}

/// R x R*
// non-optimized
Vector3f Rotor3::rotate(const Vector3f& x) const
{
	const Rotor3& p = *this;

	// q = P x
	
	auto m_x = p.a * x.GetX() + x.GetY() * p.b01 + x.GetZ() * p.b02;
	auto m_y = p.a * x.GetY() - x.GetX() * p.b01 + x.GetZ() * p.b12;
	auto m_z = p.a * x.GetZ() - x.GetX() * p.b02 - x.GetY() * p.b12;
	Vector3f q(m_x, m_y, m_z);

	const float q012 = -x.GetX() * p.b12 + x.GetY() * p.b02 - x.GetZ() * p.b01; // trivector

	// r = q P*
	
	auto r_x = p.a * q.GetX() + q.GetY() * p.b01 + q.GetZ() * p.b02 - q012 * p.b12;
	auto r_y = p.a * q.GetY() - q.GetX() * p.b01 + q012 * p.b02 + q.GetZ() * p.b12;
	auto r_z = p.a * q.GetZ() - q012 * p.b01 - q.GetX() * p.b02 - q.GetY() * p.b12;
	
	return Vector3f(r_x, r_y, r_z);
}

// Rotor3-Rotor3 product
inline Rotor3 Rotor3::operator*=(const Rotor3& r)
{
	(*this) = (*this) * r;
	return *this;
}

// rotate a rotor by another
inline Rotor3 Rotor3::rotate(const Rotor3& r) const
{
	// should unwrap this for efficiency
	return (*this) * r * (*this).reverse();
}


// Equivalent to conjugate
inline Rotor3 Rotor3::reverse() const
{
	return Rotor3(a, -b01, -b02, -b12);
}
// Length Squared
inline float Rotor3::lengthsqrd() const
{
	return sqrtf(a) + sqrtf(b01) + sqrtf(b02) + sqrtf(b12);
}
// Length
inline float Rotor3::length() const
{
	return sqrt(lengthsqrd());
}
// Normalize
inline void Rotor3::normalize()
{
	float l = length();
	a /= l; b01 /= l; b02 /= l; b12 /= l;
}
// Normalized rotor
inline Rotor3 Rotor3::normal() const
{
	Rotor3 r = *this;
	r.normalize();
	return r;
}

// convert to matrix
// non-optimized
//inline Matrix3 Rotor3::toMatrix3() const
//{
//	Vector3 v0 = rotate(Vector3(1, 0, 0));
//	Vector3 v1 = rotate(Vector3(0, 1, 0));
//	Vector3 v2 = rotate(Vector3(0, 0, 1));
//	return Matrix3(v0, v1, v2);
//}

// geometric product (for reference), produces twice the angle, negative direction
inline Rotor3 Geo(const Vector3f & a, const Vector3f & b)
{
	return Rotor3(a.dot(b), Wedge(a, b));
}
