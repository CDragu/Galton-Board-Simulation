#pragma once

#include "ContactManifold.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <string>
#include "Shape.h"
#include "Cube.h"
#include "Rotor3f.h"
#include <AntTweakBar.h>


//This will be our only Rigidbody object
class Sphere : public Shape
{
public:
	Sphere();
	~Sphere( void);

	void AddLinearImpulse(const Vector3& impulse);

	void CalculatePhysics(float dt);
	
	void CollisionWithSphere(Sphere* sphere2, ContactManifold* contactManifold);
	void CollisionWithCubeWithAxisSeparation(Cube* cube, ContactManifold* contact_manifold);
	void CollisionWithCube(Cube* cube, ContactManifold* contactManifold);
	
	void Update();
	
	void CollisionResponseWithSphere(Sphere &one, Sphere &two, Vector3 colNormal);
	void CollisionResponseWithCube(Sphere &one, Cube &two, Vector3 colNormal);

	float InvertMass();

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

	float m_friction;
	float m_elasticity; //how much bounce;
	float m_restitution; //coefficient of restitution

	Vector3 m_newPos;
	Vector3 m_velocity;
	Vector3 m_newVelocity;
	TwBar* BarObj;
};
