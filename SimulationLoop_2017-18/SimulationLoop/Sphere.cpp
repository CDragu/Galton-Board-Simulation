#include "Sphere.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <gl\GLU.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include "TextureLoader.h"
#include "Rotor3f.h"

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

void Sphere::SetRadius(float radius)
{
	m_radius = radius;
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

void Sphere::CollisionWithCube(Cube* cube, ContactManifold* contactManifold)
{
	//Vector3f PointOfSphere = this->GetNewPos() + ((this->GetNewPos() - cube->GetPos()).normalise())*this->GetRadius();
	const Vector3f SphereCenter = this->GetNewPos();
	Vector3f Normal(0, 1, 0);

	//Calculating the orthonormal vectors of our box
	Vector3f rotationX(1, 0, 0);
	if(cube->GetRot().GetX() != 0)
	{
		Rotor3 *rotx = new Rotor3(cube->GetRot().GetX(), Bivector3(1, 0, 0));
		rotationX = rotx->rotate(rotationX);
		Normal = rotx->rotate(Normal);
	}else
	{
		rotationX = Vector3f(0, 0, 0);
	}
	
	
	Vector3f rotationY(0, 1, 0);
	if(cube->GetRot().GetY() != 0 )
	{
		Rotor3 *roty = new Rotor3(cube->GetRot().GetY(), Bivector3(0, 1, 0));
		rotationY = roty->rotate(rotationY);
		Normal = roty->rotate(Normal);
	}else
	{
		rotationY = Vector3f(0, 0, 0);
	}
	

	Vector3f rotationZ(0, 0, 1);
	if(cube->GetRot().GetZ() != 0)
	{
		Rotor3 *rotz = new Rotor3(cube->GetRot().GetZ(), Bivector3(0, 0, 1));
		rotationZ = rotz->rotate(rotationZ);
		Normal = rotz->rotate(Normal);
	}else
	{
		rotationZ = Vector3f(0, 0, 0);
	}
	
	//rotationZ = rotationZ + cube->GetPos();

	//Checking collision
	auto D = (SphereCenter - cube->GetPos());
	//auto D = P - cube->GetPos();
	const auto S0 = rotationX.dot(D);
	const auto S1 = rotationY.dot(D);
	const auto S2 = rotationZ.dot(D);

	const auto e0 = cube->GetLength() / 2.0f;
	const auto e1 = cube->GetHeight() / 2.0f;
	const auto e2 = cube->GetWidth() / 2.0f;

	if(S0 <= -e0)
	{
		D = D + e0 * rotationX;
	}else if( S0 < e0)
	{
		D = D - S0 * rotationX;
	}else
	{
		D = D - e0 * rotationX;
	}


	if (S1 <= -e1)
	{
		D = D + e1 * rotationY;
	}
	else if (S1 < e1)
	{
		D = D - S1 * rotationY;
	}
	else
	{
		D = D - e1 * rotationY;
	}


	if (S2 <= -e2)
	{
		D = D + e2 * rotationZ;
	}
	else if (S2 < e2)
	{
		D = D - S2 * rotationZ;
	}
	else
	{
		D = D - e2 * rotationZ;
	}

	//Calculating distance
	if(sqrt(D.dot(D)) < this->GetRadius())
	{
		auto n = sqrt(D.dot(D));
		D = Vector3f(0,0,0);


		ManifoldPoint mp;
		mp.contactID1 = this;
		mp.contactID2 = cube;
		mp.contactNormal = (Normal).normalise();
		contactManifold->Add(mp);
	}else
	{
		auto n = sqrt(D.dot(D));
		D = Vector3f(0, 0, 0);
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

void Sphere::CollisionResponseWithSphere(Sphere &one, Sphere &two, Vector3f colNormal)
{
	// ****************************************************************
	// ******** This implementation is very basic and does not ********
	// ******** accurately model responses between Spheres     ********
	// ****************************************************************
	// ******** Replace with better response code for Spheres  ********
	// ****************************************************************

	one.ResetPos();
	one.SetNewVel(-1.0f*colNormal*colNormal.dot(one.GetVel()));

	two.ResetPos();
	two.SetNewVel(-1.0f*colNormal*colNormal.dot(two.GetVel()));
}

void Sphere::CollisionResponseWithCube(Sphere& one, Cube& two, Vector3f colNormal)
{
	one.ResetPos();
	one.SetNewVel(-1.0f * colNormal * colNormal.dot(one.GetVel()));
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
		glTranslatef(GetPos().GetX(), GetPos().GetY(), GetPos().GetZ());
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


