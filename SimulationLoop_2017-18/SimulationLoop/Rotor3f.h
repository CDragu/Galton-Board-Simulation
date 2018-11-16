#pragma once
#include "d3d11.h"
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

class Rotor3f 
{
public:
	Rotor3f();
	~Rotor3f();
};


struct Bivector3
{
	float b01 = 0;
	float b02 = 0;
	float b12 = 0;

	Bivector3(float b01, float b02, float b12);
};
struct Rotor3
{
	// scalar part
	float a = 1;
	// bivector part
	float b01 = 0;
	float b02 = 0;
	float b12 = 0;

	// default ctor
	Rotor3() {}
	Rotor3(float a, float b01, float b02, float b12);

	// construct the rotor that rotates one vector to another
	Rotor3(const Vector3& vFrom, const Vector3& vTo);
	// angle+axis, or rather angle+plane
	Rotor3(float angleRadian, const Bivector3& bvPlane);

	// rotate a vector by the rotor
	Vector3 rotate(const Vector3& v) const;

	// multiply
	Rotor3 operator*(const Rotor3& r) const;
	Rotor3 operator*=(const Rotor3& r);
	Rotor3 rotate(const Rotor3& r) const;

	// length utility
	Rotor3 reverse() const; // equivalent to conjugate
	float lengthsqrd() const;
	float length() const;
	void normalize();
	Rotor3 normal() const;

	// convert to matrix
	//Matrix3 toMatrix3() const; // TODO: we need this
};
