#include "Sphere.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <gl\GLU.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "Rotor3f.h"
#include <iostream>

Sphere::Sphere(void) : Shape(), m_mass(1), m_radius(5){}

Sphere::~Sphere(void)
= default;

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
	//TOdo: remove
	Vector3 t_scale;
	Vector3 t_pos;
	Quaternion t_rot;
	
	cube->transform.Decompose(t_scale, t_rot, t_pos);
	std::cout <<
		std::endl << std::endl <<
		std::to_string(t_pos.x) << " " <<
		std::to_string(t_pos.y) << " " <<
		std::to_string(t_pos.z) << " " << std::endl<<
		std::to_string(t_scale.x) << " " <<
		std::to_string(t_scale.y) << " " <<
		std::to_string(t_scale.z) << " " << std::endl <<
		std::to_string(t_rot.x) << " " <<
		std::to_string(t_rot.y) << " " <<
		std::to_string(t_rot.z) << " " <<
		std::to_string(t_rot.w) << " " <<
		std::endl << std::endl;


	Vector3 center = m_pos;
	Matrix inverseDX = DirectX::XMMatrixInverse(nullptr, cube->transform);
	Vector3 ballInCubeSpace = Vector3::Transform(center, inverseDX);
	Vector3 halfSize = Vector3(cube->m_length/2, cube->m_height/2, cube->m_width/2);
	
	//Todo: remove
	if(this->m_objectID == 0)
	{
		std::cout<<"Ball in Cube Space: " << std::to_string(ballInCubeSpace.x) << " " << std::to_string(ballInCubeSpace.y) << " " << std::to_string(ballInCubeSpace.z) << std::endl;
	}
	
	// Early-out check to see if we can exclude the contact.
	if (abs(ballInCubeSpace.x) - this->m_radius > halfSize.x ||
		abs(ballInCubeSpace.y) - this->m_radius > halfSize.y ||
		abs(ballInCubeSpace.z) - this->m_radius > halfSize.z)
	{
		return;
	}
	Vector3 closestPt;
	float dist;

	dist = ballInCubeSpace.x;
	if (dist > halfSize.x) dist = halfSize.x;
	if (dist < -halfSize.x) dist = -halfSize.x;
	closestPt.x = dist;
	
	dist = ballInCubeSpace.y;
	if (dist > halfSize.y) dist = halfSize.y;
	if (dist < -halfSize.y) dist = -halfSize.y;
	closestPt.y = dist;

	dist = ballInCubeSpace.z;
	if (dist > halfSize.z) dist = halfSize.z;
	if (dist < -halfSize.z) dist = -halfSize.z;
	closestPt.z = dist;

	// Check we’re in contact.
	dist = Vector3::DistanceSquared(closestPt, ballInCubeSpace);

	//Todo: remove
	if (this->m_objectID == 0)
	{
		std::cout << std::to_string(dist) << std::endl << "Closest Point(Point of impact) : " <<
			std::to_string(closestPt.x) << " " <<
			std::to_string(closestPt.y) << " " <<
			std::to_string(closestPt.z) << " " <<
			std::endl << std::endl;
	}

	if (dist > (this->m_radius * this->m_radius)) {
		return;
	}

	Vector3 closestPtWorld = closestPtWorld.Transform(closestPt, cube->transform);

	ManifoldPoint mp;
	mp.contactID1 = this;
	mp.contactID2 = cube;
	auto contactNormal = (center - closestPtWorld);
	(contactNormal).Normalize(mp.contactNormal);
	mp.contactPoint = closestPtWorld;
	mp.penetration = this->m_radius - sqrt(dist);
	contact_manifold->Add(mp);

	//Todo: solving penetration fast
	m_pos += contactNormal * mp.penetration;
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


