#pragma once

#include "Shape.h"
#include <AntTweakBar.h>

class Cylinder : public Shape
{
public:
	Cylinder();
	~Cylinder();

	void SetRadius(float p_radius);
	void SetHeight(float p_height);
	
	float GetRadius() const;
	float GetHeight() const;

	void Render() const override;
	float InvertMass();

	float m_radius;
	float m_height;

	float m_mass;

	float m_friction;
	float m_restitution; //coefficient of restitution, inverse of elasticity, how much energy remains after one collision

	Vector3 m_angularVelocity;
	Vector3 m_newAngularVelocity;
	Vector3 m_torque;
	Matrix m_InvTensor;

	Vector3 m_newPos;
	Vector3 m_newRot;
	Vector3 m_velocity;
	Vector3 m_newVelocity;

	TwBar* BarObj;
};
