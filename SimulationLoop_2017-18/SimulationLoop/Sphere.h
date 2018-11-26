#pragma once

#include "ContactManifold.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <string>
#include "Shape.h"
#include "Cube.h"
#include "Rotor3f.h"
#include <AntTweakBar.h>
#include "Cylinder.h"


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
	void CollisionWithCylinder(Cylinder* cylinder, ContactManifold* contactManifold);//TODO: implement

	void Update();
	
	void CollisionResponseWithSphere(Sphere &one, Sphere &two, Vector3 colNormal, Vector3 colPoint);
	void CollisionResponseWithCubeNoAngular(Sphere& one, Cube& two, Vector3 colNormal, Vector3 colPoint);
	void CollisionResponseWithCube(Sphere &one, Cube &two, Vector3 colNormal, Vector3 colPoint);
	void CollisionResponseWithCylinder(Sphere &one, Cylinder &two, Vector3 colNormal, Vector3 colPoint);//TODO: implement

	float InvertMass();
	void DefineInvTensor();

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
