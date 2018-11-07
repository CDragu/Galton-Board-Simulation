#pragma once

#include "ContactManifold.h"
#include "Vector3f.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <string>
#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere(void);
	~Sphere( void);

	//void SetName(std::string name);

	void CalculatePhysics(float dt);
	void CollisionWithSphere(Sphere* sphere, ContactManifold *contactManifold);
	void Update();
	void CollisionResponseWithSphere(ManifoldPoint &point);
	//std::string GetName() const;
	//void SetPos(float x, float y, float z);
	void SetVel(float x, float y, float z);
	void SetNewPos(Vector3f pos);
	void SetNewVel(Vector3f vel);
	void SetMass(float mass);

	//Vector3f GetPos() const;
	Vector3f GetNewPos() const;
	Vector3f GetVel() const;
	Vector3f GetNewVel() const;
	float GetMass() const;
	void ResetPos();
	float GetRadius() const;

	void Render() const override;

private:
	//std::string m_name;
	float m_mass;
	float m_radius;
	//Vector3f m_pos;
	Vector3f m_newPos;
	Vector3f m_velocity;
	Vector3f m_newVelocity;
	//int m_objectID;
	//GLuint m_texture;

	//static int countID;
};

