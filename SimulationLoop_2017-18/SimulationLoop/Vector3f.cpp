#include "Vector3f.h"
#define _USE_MATH_DEFINES
#include <math.h>

Vector3f::Vector3f()
{
}

Vector3f::Vector3f(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

Vector3f::~Vector3f()
= default;

void Vector3f::Set(const float x, const float y, const float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

float Vector3f::GetX() const
{
	return m_x;
}

float Vector3f::GetY() const
{
	return m_y;
}

float Vector3f::GetZ() const
{
	return m_z;
}

Vector3f Vector3f::add(const Vector3f & vec) const
{
	return Vector3f(m_x+vec.GetX(), m_y + vec.GetY(), m_z + vec.GetZ());
}

Vector3f Vector3f::subtract(const Vector3f & vec) const
{
	return Vector3f(m_x - vec.GetX(), m_y - vec.GetY(), m_z - vec.GetZ());
}

Vector3f Vector3f::mult(const float n) const
{
	return Vector3f(m_x * n, m_y * n, m_z * n);
}

Vector3f Vector3f::divide(float n) const
{
	return Vector3f(m_x / n, m_y / n, m_z / n);
}

float Vector3f::dot(const Vector3f& vec) const
{
	return m_x * vec.GetX() + m_y * vec.GetY() + m_z * vec.GetZ();
}

Vector3f Vector3f::cross(const Vector3f& vec) const
{
	float x = m_y * vec.GetZ() - m_z * vec.GetY();
	float y = m_z * vec.GetX() - m_x * vec.GetZ();
	float z = m_x * vec.GetY() - m_y * vec.GetX();

	return Vector3f(x, y, z);
}

float Vector3f::length() const
{
	return sqrt(m_x*m_x + m_y * m_y + m_z*m_z);
}

float Vector3f::distance(const Vector3f& vec) const
{
	return subtract(vec).length();
}

Vector3f Vector3f::normalise()
{
	float len = length();
	Set(m_x / len, m_y / len, m_z / len);
	return *this;
}

float Vector3f::DistanceToLine(const Vector3f& v1, const Vector3f& v2) const
{
	return fabs(((*this - v1).cross(*this - v2).length()) / (v2 - v1).length());
}

//float Vector3f::SquareDistancePointSolidBox(Cube c, Vector3f v)
//{
//	auto D = v - c.GetPos();
//	return 0.0f;
//	//auto s0 =
//}

Vector3f operator+ (const Vector3f &lhs, const Vector3f &rhs)
{
	return lhs.add(rhs);
}

Vector3f operator- (const Vector3f &lhs, const Vector3f &rhs)
{
	return lhs.subtract(rhs);
}

Vector3f operator* (const Vector3f &lhs, float n)
{
	return lhs.mult(n);
}

Vector3f operator* (float n, const Vector3f &rhs)
{
	return rhs.mult(n);
}

Vector3f operator/ (const Vector3f &lhs, float n)
{
	return lhs.divide(n);
}


