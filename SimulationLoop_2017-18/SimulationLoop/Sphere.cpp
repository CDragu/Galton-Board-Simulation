#include "Sphere.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <gl\GLU.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "Rotor3f.h"
#include <iostream>


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

void Sphere::SetNewPos(Vector3 pos)
{
	m_newPos = pos;
}

void Sphere::SetVel(float x, float y, float z)
{
	m_velocity = Vector3(x, y, z);
}

void Sphere::SetNewVel(Vector3 vel)
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
	double AngularVelocity;
	double Torque;



	// Calcuate total force for this sphere, e.g. F = F1+F2+F3+...
	Vector3 force(0.0f, -9.81f * m_mass, 0.0f);

	// Calculate acceleration
	Vector3 accel = force / m_mass;

	// Integrate accel to get the new velocity (using Euler)
	m_newVelocity = m_velocity + (accel * dt);

	// Integrate old velocity to get the new position (using Euler)
	m_newPos = GetPos() + (m_velocity * dt);

}

void Sphere::CollisionWithSphere(Sphere* sphere2, ContactManifold *contactManifold)
{
	// ****************************************************************
	// ******** This implementation is very basic and does not ********
	// ******** accurately model collisions between spheres ********
	// ****************************************************************
	// ******** Replace with better collision detection code ********
	// ****************************************************************
	Vector3 pos1 = this->GetNewPos();
	Vector3 pos2 = sphere2->GetNewPos();

	Vector3 midline = pos1 - pos2;
	float size = midline.Length();
	if (size <= 0.0f || size >= m_radius + sphere2->m_radius)
	{
		return;
	}

	Vector3 normal = midline * ((1.0f) / size);
	float penetration = m_radius + sphere2->m_radius - size;
	Vector3 contactPoint = (pos1 + midline)/2.0f;//TODO: Check

	ManifoldPoint mp;
	mp.contactID1 = this;
	mp.contactID2 = sphere2;
	Vector3 x = pos2 - pos1;
	x.Normalize(mp.contactNormal);
	//mp.contactNormal = (pos2 - pos1).normalise();
	mp.penetration = penetration;
	mp.contactPoint = contactPoint;
	contactManifold->Add(mp);
}

void Sphere::CollisionWithCubeWithAxisSeparation(Cube* cube, ContactManifold* contact_manifold)
{
	Vector3 centerOfBox = cube->GetPos();

	//Eigen::Transform<float, 3, Eigen::Affine> t = 
	//	Eigen::Translation3f(cube->GetPos().m_x, cube->GetPos().m_y, cube->GetPos().m_z)
	//	/* Eigen::AngleAxisf(1, ) */
	//	* Eigen::Scaling(1.0f);

	//t.inverse()

	////Vector3f realCenter = cube.transform.transformeInverse(center);

	//// Early-out check to see if we can exclude the contact.
	//if (abs(cube->GetPos().GetX()) - this->m_radius > box.halfSize.x ||
	//	abs(cube->GetPos().GetY() - this->m_radius > box.halfSize.y ||
	//	abs(cube->GetPos().GetZ() - this->m_radius > box.halfSize.z)
	//{
	//	return;
	//}

}

void Sphere::CollisionWithCube(Cube* cube, ContactManifold* contactManifold)
{
	//Vector3f PointOfSphere = this->GetNewPos() + ((this->GetNewPos() - cube->GetPos()).normalise())*this->GetRadius();
	const Vector3 SphereCenter = this->GetNewPos();
	Vector3 Normal(0, 1, 0);

	//Calculating the orthonormal vectors of our box
	Vector3 rotationX(1, 0, 0);
	if(cube->GetRot().x != 0)
	{
		Rotor3 *rotx = new Rotor3(cube->GetRot().x, Bivector3(1, 0, 0));
		rotationX = rotx->rotate(rotationX);
		Normal = rotx->rotate(Normal);
	}else
	{
		rotationX = Vector3(0, 0, 0);
	}
	
	
	Vector3 rotationY(0, 1, 0);
	if(cube->GetRot().y != 0 )
	{
		Rotor3 *roty = new Rotor3(cube->GetRot().y, Bivector3(0, 1, 0));
		rotationY = roty->rotate(rotationY);
		Normal = roty->rotate(Normal);
	}else
	{
		rotationY = Vector3(0, 0, 0);
	}
	

	Vector3 rotationZ(0, 0, 1);
	if(cube->GetRot().z != 0)
	{
		Rotor3 *rotz = new Rotor3(cube->GetRot().z, Bivector3(0, 0, 1));
		rotationZ = rotz->rotate(rotationZ);
		Normal = rotz->rotate(Normal);
	}else
	{
		rotationZ = Vector3(0, 0, 0);
	}
	
	//rotationZ = rotationZ + cube->GetPos();

	//Checking collision
	auto D = (SphereCenter - cube->GetPos());
	//auto D = P - cube->GetPos();
	const auto S0 = rotationX.Dot(D);
	const auto S1 = rotationY.Dot(D);
	const auto S2 = rotationZ.Dot(D);

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
	if(sqrt(D.Dot(D)) < this->GetRadius())
	{
		auto n = sqrt(D.Dot(D));
		D = Vector3(0,0,0);


		ManifoldPoint mp;
		mp.contactID1 = this;
		mp.contactID2 = cube;
		(Normal).Normalize(mp.contactNormal);
		contactManifold->Add(mp);
	}else
	{
		auto n = sqrt(D.Dot(D));
		//std::cout << std::to_string(n) << std::endl;
		D = Vector3(0, 0, 0);
	}


}

void Sphere::ResetPos()
{
	m_newPos = GetPos();
}

void Sphere::Update()
{
	m_velocity = m_newVelocity;
	SetPos(m_newPos.x, m_newPos.y, m_newPos.z);
}

void Sphere::CollisionResponseWithSphere(Sphere &one, Sphere &two, Vector3 colNormal)
{
	// ****************************************************************
	// ******** This implementation is very basic and does not ********
	// ******** accurately model responses between Spheres     ********
	// ****************************************************************
	// ******** Replace with better response code for Spheres  ********
	// ****************************************************************

	one.ResetPos();
	one.SetNewVel(-1.0f*colNormal*colNormal.Dot(one.GetVel()));

	two.ResetPos();
	two.SetNewVel(-1.0f*colNormal*colNormal.Dot(two.GetVel()));
}

void Sphere::CollisionResponseWithCube(Sphere& one, Cube& two, Vector3 colNormal)
{
	one.ResetPos();
	one.SetNewVel(-1.0f * colNormal * colNormal.Dot(one.GetVel()));
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

Vector3 Sphere::GetNewPos() const
{
	return m_newPos;
}

Vector3 Sphere::GetVel() const
{
	return m_velocity;
}

Vector3 Sphere::GetNewVel() const
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
		glTranslatef(GetPos().x, GetPos().y, GetPos().z);
		glRotatef(GetRot().x, 1, 0, 0);
		glRotatef(GetRot().y, 0, 1, 0);
		glRotatef(GetRot().z, 0, 0, 1);
		glColor3d(1, 0, 0); // Here to change Color
		glBindTexture(GL_TEXTURE_2D, GetTexture());               // Select Our Texture, Maybe bad for performance
		GLUquadric *quadric = gluNewQuadric();
		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluQuadricTexture(quadric, GL_TRUE); 
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluSphere(quadric, m_radius, 20, 20);
		
	glPopMatrix();
}


