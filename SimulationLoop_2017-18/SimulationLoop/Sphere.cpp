#include "Sphere.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <gl\GLU.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "TextureLoader.h"

//int Sphere::countID = 0;

Sphere::Sphere(void) : Shape(), m_mass(1), m_radius(5)
{
	//m_objectID = countID;
	//++countID;
	//m_texture = TextureLoader::LoadBMP("checker.bmp");
}

Sphere::~Sphere(void)
{
}

//void Sphere::SetName(std::string name)
//{
//	m_name = name;
//}

//void Sphere::SetPos(float x, float y, float z)
//{
//	m_pos.Set(x, y, z);
//}

void Sphere::SetNewPos(Vector3f pos)
{
	m_newPos = pos;
}

void Sphere::SetVel(float x, float y, float z)
{
	m_velocity.Set(x, y, z);
}

void Sphere::SetNewVel(Vector3f vel)
{
	m_newVelocity = vel;
}

void Sphere::SetMass(float mass)
{
	m_mass = mass;
}

void Sphere::CalculatePhysics(float dt)
{
	// Calcuate total force for this sphere, e.g. F = F1+F2+F3+...
	Vector3f force(0.0f, -9.81f * m_mass, 0.0f);

	// Calculate acceleration
	Vector3f accel = force / m_mass;

	// Integrate accel to get the new velocity (using Euler)
	m_newVelocity = m_velocity + (accel * dt);

	// Integrate old velocity to get the new position (using Euler)
	m_newPos = GetPos() + (m_velocity * dt);

	// ****************************************************************
	// ******** This implementation is very basic and does not ********
	// ******** accurately model collisions with a plane       ********
	// ****************************************************************
	// ******** Replace with better collision detection code   ********
	// ****************************************************************
	/*if(m_newPos.GetY() < -20.0f+m_radius)
	{
		m_newPos.Set(m_newPos.GetX(), -20.0f + m_radius, m_newPos.GetZ());
		m_newVelocity.Set(m_newVelocity.GetX(), 0.0f, 0.0f);
	}*/
}

void Sphere::CollisionWithSphere(Sphere* sphere2, ContactManifold *contactManifold)
{
	// ****************************************************************
	// ******** This implementation is very basic and does not ********
	// ******** accurately model collisions between spheres ********
	// ****************************************************************
	// ******** Replace with better collision detection code ********
	// ****************************************************************
	Vector3f pos1 = this->GetNewPos();
	Vector3f pos2 = sphere2->GetNewPos();
	float dist = pos1.distance(pos2) - (this->GetRadius() + sphere2->GetRadius());
	if (dist < 0.0f)
	{
		ManifoldPoint mp;
		mp.contactID1 = this;
		mp.contactID2 = sphere2;
		mp.contactNormal = (pos2 - pos1).normalise();
		contactManifold->Add(mp);
	}
}

void Sphere::ResetPos()
{
	m_newPos = GetPos();
}

void Sphere::Update()
{
	m_velocity = m_newVelocity;
	SetPos(m_newPos.GetX(), m_newPos.GetY(), m_newPos.GetZ());
}

void Sphere::CollisionResponseWithSphere(ManifoldPoint &point)
{
	// ****************************************************************
	// ******** This implementation is very basic and does not ********
	// ******** accurately model responses between Spheres     ********
	// ****************************************************************
	// ******** Replace with better response code for Spheres  ********
	// ****************************************************************

	Vector3f colNormal = point.contactNormal;

	point.contactID1->ResetPos();
	point.contactID1->SetNewVel(-1.0f*colNormal*colNormal.dot(point.contactID1->GetVel()));

	point.contactID2->ResetPos();
	point.contactID2->SetNewVel(-1.0f*colNormal*colNormal.dot(point.contactID2->GetVel()));
}

//std::string Sphere::GetName()const
//{
//	return m_name;
//}

float Sphere::GetMass() const
{
	return m_mass;
}

//Vector3f Sphere::GetPos() const
//{
//	return m_pos;
//}

Vector3f Sphere::GetNewPos() const
{
	return m_newPos;
}

Vector3f Sphere::GetVel() const
{
	return m_velocity;
}

Vector3f Sphere::GetNewVel() const
{
	return m_newVelocity;
}

float Sphere::GetRadius() const
{
	return m_radius;
}

void Sphere::Render() const									
{
	glPushMatrix();
		glTranslatef(GetPos().GetX(), GetPos().GetY(), 0);
		glRotatef(GetRot().GetX(), 1, 0, 0);
		glRotatef(GetRot().GetY(), 0, 1, 0);
		glRotatef(GetRot().GetZ(), 0, 0, 1);
		glColor3d(1, 0, 0); // Here to change Color
		glBindTexture(GL_TEXTURE_2D, GetTexture());               // Select Our Texture, Maybe bad for performance
		GLUquadric *quadric = gluNewQuadric();
		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluQuadricTexture(quadric, GL_TRUE); 
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluSphere(quadric, m_radius, 20, 20);
		
	glPopMatrix();
}


