#pragma once
#include "Shape.h"
#include <AntTweakBar.h>

class Cube :
	public Shape
{
public:
	Cube(void);
	~Cube(void);

	float InvertMass();
	void DefineInvTensor();

	void SetPoints(const Vector3 p_points[8]);
	void SetSize(const float p_size);
	void SetLength(const float p_length);
	void SetHeight(const float p_height);
	void SetWidth(const float p_width);

	void GetPoints(Vector3 points[8]) const;
	float GetLength() const;
	float GetHeight() const;
	float GetWidth() const;


	void CalculatePoints();
	void Render() const override;


	Vector3 m_points[8];
	float m_size;
	float m_width;
	float m_height;
	float m_length;

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

	Vector3 m_color;

	TwBar* BarObj;
};
