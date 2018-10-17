#pragma once

#include "ContactManifold.h"
#include "Vector2f.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <string>

class Sphere
{
public:
	Sphere(void);
	~Sphere(void);

	void SetName(std::string name);

	void CalculatePhysics(float dt);
	void CollisionWithSphere(Sphere* sphere, ContactManifold *contactManifold);
	void Update();
	void CollisionResponseWithSphere(ManifoldPoint &point);
	std::string GetName() const;
	void SetPos(float x, float y);
	void SetVel(float x, float y);
	void SetNewPos(Vector2f pos);
	void SetNewVel(Vector2f vel);
	void SetMass(float mass);

	Vector2f GetPos() const;
	Vector2f GetNewPos() const;
	Vector2f GetVel() const;
	Vector2f GetNewVel() const;
	float GetMass() const;
	void ResetPos();
	float GetRadius() const;

	void Render() const;

private:
	std::string m_name;
	float m_mass;
	float m_radius;
	Vector2f m_pos;
	Vector2f m_newPos;
	Vector2f m_velocity;
	Vector2f m_newVelocity;
	int m_objectID;
	GLuint m_texture;

	static int countID;
};

