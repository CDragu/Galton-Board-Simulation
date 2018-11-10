#pragma once

#include "ContactManifold.h"
#include "Vector3f.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <string>
#include "Shape.h"
#include "Cube.h"

class Sphere : public Shape
{
public:
	Sphere(void);
	~Sphere( void);

	void CalculatePhysics(float dt);
	void CollisionWithSphere(Sphere *sphere, ContactManifold *contactManifold);
	void CollisionWithCube(Cube *cube, ContactManifold *contactManifold);
	void Update();
	void CollisionResponseWithSphere(Sphere &one, Sphere &two, Vector3f colNormal);
	void CollisionResponseWithCube(Sphere &one, Cube &two, Vector3f colNormal);
	
	void SetVel(float x, float y, float z);
	void SetNewPos(Vector3f pos);
	void SetNewVel(Vector3f vel);
	void SetMass(float mass);
	void SetRadius(float radius);

	
	Vector3f GetNewPos() const;
	Vector3f GetVel() const;
	Vector3f GetNewVel() const;
	float GetMass() const;
	void ResetPos();
	float GetRadius() const;

	void Render() const override;

private:
	float m_mass;
	float m_radius;
	
	Vector3f m_newPos;
	Vector3f m_velocity;
	Vector3f m_newVelocity;
};

