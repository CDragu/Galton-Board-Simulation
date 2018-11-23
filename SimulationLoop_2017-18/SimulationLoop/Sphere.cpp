#include "Sphere.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <gl\GLU.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "Rotor3f.h"
#include <iostream>
#include <AntTweakBar.h>
//#define GRAVITY_CONST Vector3(0.0f, -8.91f, 0.0f)
#define GRAVITY_CONST Vector3(0.0f, 0, 0.0f)

Sphere::Sphere() : Shape(), m_mass(1), m_radius(5), m_friction(0.6f), m_restitution(1.6f)
{
	DefineInvTensor();
	
	/*std::string x = std::to_string(this->countID);
	BarObj = TwNewBar(x.c_str());
	TwAddVarRW(BarObj, "X: ", TW_TYPE_FLOAT, &this->m_pos.x, "");
	TwDefine((x + "  position='0 300' ").c_str());
	TwDefine((x + "  size='100 50' ").c_str());
	TwDefine("Bar refresh=0.1 ");*/
}

Sphere::~Sphere(void)
= default;

// This is going to be used instead of the setvel method to resolve collision responses
void Sphere::AddLinearImpulse(const Vector3& impulse)
{
	m_velocity += impulse;
}

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

//Verlet integration
void Sphere::CalculatePhysics(float dt)
{
	float airResistance = 1.0f; //TODO: change

	//Position
	Vector3 force = GRAVITY_CONST * m_mass;

	Vector3 accel = force * InvertMass();
	
	m_newVelocity = m_velocity + (accel * dt);

	m_newVelocity = m_newVelocity * airResistance;
	
	
	//Roation
	Vector3 angleAccel = m_torque.Transform(m_torque, m_InvTensor);
	
	m_newAngularVelocity = m_angularVelocity + angleAccel * dt;
	
	m_newAngularVelocity = m_newAngularVelocity * airResistance;

	// Integrate old velocity to get the new position (using Verlet)
	m_newPos = GetPos() + (m_newVelocity * dt);
	m_newRot = m_rot + (m_newAngularVelocity * dt);
}

void Sphere::CollisionWithSphere(Sphere* sphere2, ContactManifold* contactManifold)
{
	Vector3 pos1 = this->GetNewPos();
	Vector3 pos2 = sphere2->GetNewPos();

	Vector3 midline = pos1 - pos2;
	float size = midline.Length();
	if (size <= 0.0f || size >= m_radius + sphere2->m_radius)
	{
		return;
	}

	float penetration = m_radius + sphere2->m_radius - size;
	//Vector3 contactPoint = (pos1 + midline)/2.0f;//TODO: Check

	ManifoldPoint mp;
	mp.contactID1 = this;
	mp.contactID2 = sphere2;
	Vector3 x = pos2 - pos1;
	x.Normalize(mp.contactNormal);
	mp.penetration = penetration;
	mp.contactPoint = (pos1 + pos2) / 2.0f;
	contactManifold->Add(mp);
	
	//Todo: solving penetration fast
	//m_pos += mp.contactNormal * mp.penetration;
}

void Sphere::CollisionWithCubeWithAxisSeparation(Cube* cube, ContactManifold* contact_manifold)
{
	Vector3 center = m_pos;
	Matrix inverseDX = DirectX::XMMatrixInverse(nullptr, cube->transform);
	Vector3 ballInCubeSpace = Vector3::Transform(center, inverseDX);
	Vector3 halfSize = Vector3(cube->m_length/2, cube->m_height/2, cube->m_width/2);
	
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
	if(m_newVelocity.LengthSquared() < 0.01f)
	{
		m_velocity *= 0;
		m_newVelocity *= 0;
		return;
	}
	m_velocity = m_newVelocity;
	m_angularVelocity = m_newAngularVelocity;
	SetPos(m_newPos.x, m_newPos.y, m_newPos.z);
	SetRot(m_newRot.x, m_newRot.y, m_newRot.z);
}

void Sphere::CollisionResponseWithSphere(Sphere &one, Sphere &two, Vector3 colNormal, Vector3 colPoint)
{
	float massOne = one.InvertMass();
	float massTwo = two.InvertMass();
	if (massOne + massTwo == 0.0f)
	{
		return;
	}

	Vector3 onePointOfContact = colPoint - one.m_pos; //TODO: FIX
	Vector3 twoPointOfContact = colPoint - two.m_pos; //TODO: FIX

	Matrix tensor1 = m_InvTensor;
	Matrix tensor2 = two.m_InvTensor;



	Vector3 relativeVelocity = (two.m_velocity + two.m_angularVelocity.Cross(twoPointOfContact)) -
		(one.m_velocity + one.m_angularVelocity.Cross(onePointOfContact));

	float DotRelativeVelocity = relativeVelocity.Dot(colNormal);
	if (DotRelativeVelocity > 0.0f) {
		return; //If the objects are moving apart then they can not be colliding
	}

	float e = fmin(one.m_restitution, two.m_restitution);
	float j = (-(1.0f + e) * DotRelativeVelocity);//Calculating the magnitude of collision

	{
		float SumMass = massOne + massTwo;
		Vector3 aux1 = onePointOfContact.Cross(colNormal);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		Vector3 aux3 = aux2.Cross(onePointOfContact);

		Vector3 aux4 = twoPointOfContact.Cross(colNormal);
		Vector3 aux5 = aux4.Transform(aux4, tensor2);
		Vector3 aux6 = aux5.Cross(twoPointOfContact);

		Vector3 aux7 = aux3 + aux6;
		float aux8 = SumMass + colNormal.Dot(aux7);

		if (aux8 != 0)
		{
			j = j / aux8;
		}

		if (j != 0)
		{
			j = j / 2.0f;
		}
	}
	Vector3 impulse = colNormal * j;
	one.SetNewVel(one.m_velocity - impulse * massOne);

	two.SetNewVel(two.m_velocity + impulse * massTwo);
	//Angular velocity
	{
		Vector3 aux1 = onePointOfContact.Cross(impulse);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		one.m_newAngularVelocity = one.m_angularVelocity - aux2;

		Vector3 aux3 = twoPointOfContact.Cross(impulse);
		Vector3 aux4 = aux3.Transform(aux3, tensor2);
		two.m_newAngularVelocity = two.m_angularVelocity + aux4;
	}

	return;

	//Adding some friction now
	Vector3 t = relativeVelocity - (colNormal * DotRelativeVelocity);
	if(t.LengthSquared() < 0.0005f)
	{
		return;
	}

	t.Normalize(t);
	float jt;
	{
		jt = -relativeVelocity.Dot(t);

		float SumMass = massOne + massTwo;
		Vector3 aux1 = onePointOfContact.Cross(t);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		Vector3 aux3 = aux2.Cross(onePointOfContact);

		Vector3 aux4 = twoPointOfContact.Cross(t);
		Vector3 aux5 = aux4.Transform(aux4, tensor2);
		Vector3 aux6 = aux5.Cross(twoPointOfContact);

		Vector3 aux7 = aux3 + aux6;
		float aux8 = SumMass + t.Dot(aux7);

		jt = (jt / aux8) / 2.0f;
	}
	//jt = (-relativeVelocity.Dot(t) / (massOne + massTwo))/2.0f;

	if (jt < 0.0005f)
	{
		return;
	}

	float friction = sqrt(one.m_friction * two.m_friction);
	if (jt > j * friction) {
		jt = j * friction;
	}
	else if (jt < -j * friction) {
		jt = -j * friction;
	}

	Vector3 TangentOfImpulse = t * jt;

	one.SetNewVel(one.m_newVelocity - TangentOfImpulse * massOne);

	two.SetNewVel(one.m_newVelocity + TangentOfImpulse * massTwo);

	//Angular velocity
	{
		Vector3 aux1 = onePointOfContact.Cross(TangentOfImpulse);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		one.m_newAngularVelocity = one.m_angularVelocity - aux2;

		Vector3 aux3 = twoPointOfContact.Cross(TangentOfImpulse);
		Vector3 aux4 = aux3.Transform(aux3, tensor2);
		two.m_newAngularVelocity = two.m_angularVelocity + aux4;
	}
}

void Sphere::CollisionResponseWithCubeNoAngular(Sphere& one, Cube& two, Vector3 colNormal, Vector3 colPoint)
{
	float massOne = one.InvertMass();

	Vector3 relativeVelocity = one.m_velocity;
	float DotRelativeVelocity = relativeVelocity.Dot(colNormal);
	if (DotRelativeVelocity > 0.0f) 
	{
		return; //If the objects are moving apart then they can not be colliding
	}

	float e = one.m_restitution;
	float j = ((-(1.0f + e) * DotRelativeVelocity) / (massOne));//Calculating the magnitude of collision

	Vector3 impulse = colNormal * j;
	one.SetNewVel(one.m_velocity + impulse * massOne);

	

	//Adding some friction now
	Vector3 t = relativeVelocity - (colNormal * DotRelativeVelocity);
	if (t.LengthSquared() < 0.0005f)
	{
		return;
	}

	t.Normalize(t);
	float jt = (-relativeVelocity.Dot(t) / (massOne)) ;

	if (abs(jt) < 0.0005f)
	{
		return;
	}

	float friction = sqrt(one.m_friction * 1);
	if (jt > (j * friction)) 
	{
		jt = j * friction;
	}
	else if (jt < (-j * friction)) 
	{
		jt = -j * friction;
	}

	Vector3 TangentOfImpulse = t * jt;

	one.SetNewVel(one.m_newVelocity - TangentOfImpulse * massOne);
}

void Sphere::CollisionResponseWithCube(Sphere& one, Cube& two, Vector3 colNormal, Vector3 colPoint)
{
	float massOne = one.InvertMass();
	float massTwo = two.InvertMass();
	if (massOne + massTwo == 0.0f)
	{
		return;
	}

	Vector3 onePointOfContact = colPoint - one.m_pos;
	Vector3 twoPointOfContact = colPoint - two.m_pos;

	Matrix tensor1 = m_InvTensor;
	Matrix tensor2 = two.m_InvTensor;


	Vector3 relativeVelocity = (two.m_velocity + two.m_angularVelocity.Cross(twoPointOfContact)) 
		- (one.m_velocity + one.m_angularVelocity.Cross(onePointOfContact));


	{
		auto velocity2 = (two.m_velocity + two.m_angularVelocity.Cross(twoPointOfContact));
		auto velocity1 = (one.m_velocity + one.m_angularVelocity.Cross(onePointOfContact));

		relativeVelocity = velocity2  - velocity1;
	}
	colNormal.Normalize();
	
	float DotRelativeVelocity = relativeVelocity.Dot(colNormal);
	if (DotRelativeVelocity > 0.0f) {
		return; //If the objects are moving apart then they can not be colliding
	}

	float e = fmin(one.m_restitution, two.m_restitution);
	float j = (-(1.0f + e) * DotRelativeVelocity);//Calculating the magnitude of collision

	{
		float SumMass = massOne + massTwo;
		Vector3 aux1 = onePointOfContact.Cross(colNormal);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		Vector3 aux3 = aux2.Cross(onePointOfContact);

		Vector3 aux4 = twoPointOfContact.Cross(colNormal);
		Vector3 aux5 = aux4.Transform(aux4, tensor2);
		Vector3 aux6 = aux5.Cross(twoPointOfContact);

		Vector3 aux7 = aux3 + aux6;
		float aux8 = SumMass + colNormal.Dot(aux3) + colNormal.Dot(aux6);

		if (aux8 != 0)
		{
			j = j / aux8;
		}

		if (j != 0)
		{
			j = j / 2.0f;
		}
	}
	Vector3 impulse = colNormal * j;
	one.SetNewVel(one.m_velocity - impulse * massOne);

	//two.SetNewVel(two.m_velocity + impulse * massTwo);
	//Angular velocity
	{
		Vector3 aux1 = onePointOfContact.Cross(impulse);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		one.m_newAngularVelocity = one.m_angularVelocity - aux2;

	/*	Vector3 aux3 = twoPointOfContact.Cross(impulse);
		Vector3 aux4 = aux3.Transform(aux3, tensor2);
		two.m_newAngularVelocity = two.m_angularVelocity + aux4;*/
	}
	return;

	//Adding some friction now
	Vector3 t = relativeVelocity - (colNormal * DotRelativeVelocity);
	if (t.LengthSquared() < 0.0005f)
	{
		return;
	}

	t.Normalize(t);
	float jt;
	{
		jt = -relativeVelocity.Dot(t);

		float SumMass = massOne + massTwo;
		Vector3 aux1 = onePointOfContact.Cross(t);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		Vector3 aux3 = aux2.Cross(onePointOfContact);

		Vector3 aux4 = twoPointOfContact.Cross(t);
		Vector3 aux5 = aux4.Transform(aux4, tensor2);
		Vector3 aux6 = aux5.Cross(twoPointOfContact);

		Vector3 aux7 = aux3 + aux6;
		float aux8 = SumMass + t.Dot(aux7);

		jt = (jt / aux8) / 2.0f;
	}
	//jt = (-relativeVelocity.Dot(t) / (massOne + massTwo))/2.0f;

	if (jt < 0.0005f)
	{
		return;
	}

	float friction = sqrt(one.m_friction * two.m_friction);
	if (jt > j * friction) {
		jt = j * friction;
	}
	else if (jt < -j * friction) {
		jt = -j * friction;
	}

	Vector3 TangentOfImpulse = t * jt;

	one.SetNewVel(one.m_newVelocity - TangentOfImpulse * massOne);

	//two.SetNewVel(one.m_newVelocity + TangentOfImpulse * massTwo);

	//Angular velocity
	{
		Vector3 aux1 = onePointOfContact.Cross(TangentOfImpulse);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		one.m_newAngularVelocity = one.m_angularVelocity - aux2;

		/*Vector3 aux3 = twoPointOfContact.Cross(TangentOfImpulse);
		Vector3 aux4 = aux3.Transform(aux3, tensor2);
		two.m_newAngularVelocity = two.m_angularVelocity + aux4;*/
	}
}

float Sphere::InvertMass()
{
	if(m_mass == 0.0f)
	{
		return 0.0f;
	}
	else
	{
		return 1.0f / m_mass;
	}
}

void Sphere::DefineInvTensor()
{
	float x, y, z, w = 0;

	float rSquare = m_radius * m_radius;

	x = rSquare * m_mass * (2.0f / 5.0f);
	y = x;
	z = x;
	w = 1;

	m_InvTensor = Matrix(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, w).Invert();
}
 
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
		float degreeX = DirectX::XMConvertToDegrees(GetRot().x);
		float degreeY = DirectX::XMConvertToDegrees(GetRot().y);
		float degreeZ = DirectX::XMConvertToDegrees(GetRot().z);
		glRotatef(degreeX, 1, 0, 0);
		glRotatef(degreeY, 0, 1, 0);
		glRotatef(degreeZ, 0, 0, 1);
		glColor3d(1, 0, 0); // Here to change Color
		glBindTexture(GL_TEXTURE_2D, GetTexture());               // Select Our Texture, Maybe bad for performance
		GLUquadric *quadric = gluNewQuadric();
		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluQuadricTexture(quadric, GL_TRUE); 
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluSphere(quadric, m_radius, 20, 20);
		
	glPopMatrix();
}


