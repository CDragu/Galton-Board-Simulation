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
	return Vector3f(this->GetX()+vec.GetX(), this->GetY() + vec.GetY(), this->GetZ() + vec.GetZ());
}

Vector3f Vector3f::subtract(const Vector3f & vec) const
{
	return Vector3f(this->GetX() - vec.GetX(), this->GetY() - vec.GetY(), this->GetZ() - vec.GetZ());
}

Vector3f Vector3f::mult(const float n) const
{
	return Vector3f(this->GetX() * n, this->GetY() * n, this->GetZ() * n);
}


