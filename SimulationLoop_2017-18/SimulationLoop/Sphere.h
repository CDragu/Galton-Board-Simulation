#pragma once

#include "ContactManifold.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <string>
#include "Shape.h"
#include "Cube.h"
#include "Rotor3f.h"


class Sphere : public Shape
{
public:
	Sphere(void);
	~Sphere( void);

	void CalculatePhysics(float dt);
	void CollisionWithSphere(Sphere* sphere2, ContactManifold* contactManifold);
	void CollisionWithCubeWithAxisSeparation(Cube* cube, ContactManifold* contact_manifold);
	void CollisionWithCube(Cube* cube, ContactManifold* contactManifold);
	void Update();
	void CollisionResponseWithSphere(Sphere &one, Sphere &two, Vector3 colNormal);
	void CollisionResponseWithCube(Sphere &one, Cube &two, Vector3 colNormal);
	
	void SetVel(float x, float y, float z);
	void SetNewPos(Vector3 pos);
	void SetNewVel(Vector3 vel);
	void SetMass(float mass);
	void SetRadius(float radius);

	
	Vector3 GetNewPos() const;
	Vector3 GetVel() const;
	Vector3 GetNewVel() const;
	float GetMass() const;
	void ResetPos();
	float GetRadius() const;

	void Render() const override;


	float m_mass;
	float m_radius;

	Matrix Ibody;					//inertia tensor for sphere
	Matrix IbodyInv;				//inverse of inertia tensors
	
	Rotor3 m_rotor;							//Rotor of the body

	Vector3 P;								//Linear Momentum
	Vector3 L;								//Angular Momentum

	Matrix Iinv;
	Matrix R;
	Vector3 v;								//Velocity at time t
	Vector3 omega;							//Angular velocity at time t

	Vector3 force;
	Vector3 torque;

	Vector3 m_newPos;
	Vector3 m_velocity;
	Vector3 m_newVelocity;
};
