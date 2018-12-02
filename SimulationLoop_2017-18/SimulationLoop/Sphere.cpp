#include "Sphere.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <gl\GLU.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <AntTweakBar.h>
#define GRAVITY_CONST Vector3(0.0f, -80.91f, 0.0f)
//#define GRAVITY_CONST Vector3(0.0f, 0, 0.0f)

Sphere::Sphere() : Shape(), m_mass(1), m_radius(5), m_friction(0.7f), m_restitution(1.8f), m_sleepThreshold(0.01f), m_isSleeping(false)
{
	DefineInvTensor();
	/*std::string x = std::to_string(this->countID);
	BarObj = TwNewBar(x.c_str());
	TwAddSeparator(BarObj, NULL, " group='Pos' ");
	TwAddVarRW(BarObj, "X: ", TW_TYPE_FLOAT, &this->m_pos.x, "group=Pos");
	TwAddVarRW(BarObj, "Y: ", TW_TYPE_FLOAT, &this->m_pos.y, "group=Pos");
	TwAddVarRW(BarObj, "Z: ", TW_TYPE_FLOAT, &this->m_pos.z, "group=Pos");
	
	TwAddSeparator(BarObj, NULL, " group='Vel' ");
	TwAddVarRW(BarObj, "Vel X: ", TW_TYPE_FLOAT, &this->m_velocity.x, "");
	TwAddVarRW(BarObj, "Vel Y: ", TW_TYPE_FLOAT, &this->m_velocity.y, "");
	TwAddVarRW(BarObj, "Vel Z: ", TW_TYPE_FLOAT, &this->m_velocity.z, "");
	
	TwAddSeparator(BarObj, NULL, " group='Angular Vel' ");
	TwAddVarRW(BarObj, "Angular X: ", TW_TYPE_FLOAT, &this->m_angularVelocity.x, "");
	TwAddVarRW(BarObj, "Angular Y: ", TW_TYPE_FLOAT, &this->m_angularVelocity.y, "");
	TwAddVarRW(BarObj, "Angular Z: ", TW_TYPE_FLOAT, &this->m_angularVelocity.z, "");
	TwDefine((x + "  position='0 300' ").c_str());
	TwDefine((x + "  size='300 300' ").c_str());
	TwDefine((x + "  refresh=0.1 ").c_str());*/
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

//Euler integration
void Sphere::CalculatePhysics(float dt)
{
	if(m_isSleeping)
	{
		return;
	}

	float airResistance = 0.98f;

	//Calculating the force
	Vector3 force = GRAVITY_CONST * m_mass;

	//Calculating Acceleration 
	Vector3 accel = force * InvertMass();

	//Calculating the new Velocity
	m_newVelocity = (m_velocity + (accel * dt)) * airResistance;
	
	//Calculating the Angular Acceleration
	Vector3 angleAccel = m_torque.Transform(m_torque, m_InvTensor);

	//Calculating the Angular Velocity
	m_newAngularVelocity = (m_angularVelocity + angleAccel * dt) * airResistance;

	// Integrate old velocity to get the new position (using Euler)
	m_newPos = GetPos() + (m_newVelocity * dt);
	// Integrate old rotation to get the new rotation (using Euler)
	m_newRot = m_rot + (m_newAngularVelocity * dt);

	//Tried integrating Verlet
	//m_newPos = m_pos + dt * m_velocity + ((dt*dt) / 2)*accel;
	//m_newRot = (m_rot * (dt * m_angularVelocity + ((dt*dt) / 2)*m_torque)).Cross(m_rot);
	
	
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
	
	ManifoldPoint mp;
	mp.contactID1 = this;
	mp.contactID2 = sphere2;
	Vector3 x = pos2 - pos1;
	x.Normalize(mp.contactNormal);
	mp.penetration = penetration;
	mp.contactPoint = (pos1 + pos2) / 2.0f;
	contactManifold->Add(mp);
	
}

void Sphere::CollisionWithCubeWithAxisSeparation(Cube* cube, ContactManifold* contact_manifold)
{
	Vector3 center = m_pos;

	if(center.Distance(center , cube->m_pos) > 100 * m_radius)
	{
		return;
	}


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

}

void Sphere::CollisionWithCylinder(Cylinder* cylinder, ContactManifold* contactManifold)
{
	Vector3 pos1 = this->GetNewPos();
	Vector3 pos2 = cylinder->m_pos;

	Vector3 midline = pos1 - pos2;
	midline = Vector3(midline.x, midline.y, 0); //Cylinders are always aligned on the Z axis
	float size = midline.Length();
	if (size <= 0.0f || size >= m_radius + cylinder->m_radius)
	{
		return;
	}

	float penetration = m_radius + cylinder->m_radius - size;

	ManifoldPoint mp;
	mp.contactID1 = this;
	mp.contactID2 = cylinder;
	Vector3 x = pos1 - pos2;
	x.z = 0;
	x.Normalize(mp.contactNormal);
	mp.penetration = penetration;
	auto pos2Midline = pos2;
	pos2Midline.z = pos1.z;
	mp.contactPoint = (pos1 + pos2Midline) / 2.0f;
	contactManifold->Add(mp);
}

void Sphere::ResetPos()
{
	m_newPos = GetPos();
}

void Sphere::Update()
{
	if(m_newVelocity.LengthSquared() < m_sleepThreshold)
	{
		m_velocity *= 0;
		m_newVelocity *= 0;
		m_angularVelocity *= 0;
		m_newAngularVelocity *= 0;
		m_isSleeping = true;
		return;
	}
	m_velocity = m_newVelocity;
	m_angularVelocity = m_newAngularVelocity;
	SetPos(m_newPos.x, m_newPos.y, m_newPos.z);
	SetRot(m_newRot.x, m_newRot.y, m_newRot.z);
	m_isSleeping = false;
}

void Sphere::CollisionResponseWithSphere(Sphere &one, Sphere &two, Vector3 colNormal, Vector3 colPoint)
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


	Vector3 relativeVelocity;
	{
		auto velocity2 = (two.m_velocity + two.m_angularVelocity.Cross(twoPointOfContact));
		auto velocity1 = (one.m_velocity + one.m_angularVelocity.Cross(onePointOfContact));

		relativeVelocity = velocity2  - velocity1;
		
	}
	colNormal.Normalize();
	
	float DotRelativeVelocity = relativeVelocity.Dot(-colNormal);
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
	one.SetNewVel(one.m_velocity + impulse * massOne);

	//Angular velocity
	{
		Vector3 aux1 = onePointOfContact.Cross(impulse);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		one.m_newAngularVelocity = one.m_angularVelocity + aux2; 
	}
	

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

	if (abs(jt) < 0.0005f)
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

	//Angular velocity
	{
		Vector3 aux1 = onePointOfContact.Cross(TangentOfImpulse);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		one.m_newAngularVelocity = one.m_angularVelocity - aux2;
	}
}

void Sphere::CollisionResponseWithCylinder(Sphere& one, Cylinder& two, Vector3 colNormal, Vector3 colPoint)
{
	float massOne = one.InvertMass();
	float massTwo = two.InvertMass();
	if (massOne + massTwo == 0.0f)
	{
		return;
	}

	Vector3 onePointOfContact = colPoint - one.m_pos;
	Vector3 twoPointOfContact = colPoint - two.m_pos;
	twoPointOfContact.z = 0;

	Matrix tensor1 = m_InvTensor;
	Matrix tensor2 = two.m_InvTensor;


	Vector3 relativeVelocity;
	{
		auto velocity2 = (two.m_velocity + two.m_angularVelocity.Cross(twoPointOfContact));
		auto velocity1 = (one.m_velocity + one.m_angularVelocity.Cross(onePointOfContact));

		relativeVelocity = velocity2 - velocity1;
	}
	colNormal.Normalize();
	
	float DotRelativeVelocity = relativeVelocity.Dot(-colNormal);
	if (DotRelativeVelocity > 0.0f) {
		return; //If the objects are moving apart then they can not be colliding
	}

	float e = fmin(one.m_restitution, two.m_restitution);
	float j = (-(1.0f + e) * DotRelativeVelocity);//Calculating the magnitude of collision

	{
		//Calculating the bottom part of the function
		float SumMass = massOne + massTwo;
		Vector3 aux1 = onePointOfContact.Cross(colNormal);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		Vector3 aux3 = aux2.Cross(onePointOfContact);

		Vector3 aux4 = twoPointOfContact.Cross(colNormal);
		Vector3 aux5 = aux4.Transform(aux4, tensor2);
		Vector3 aux6 = aux5.Cross(twoPointOfContact);

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
	one.SetNewVel(one.m_velocity + impulse * massOne);

	//Angular velocity
	{
		Vector3 aux1 = onePointOfContact.Cross(impulse);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		one.m_newAngularVelocity = one.m_angularVelocity + aux2;
	}


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

	if (abs(jt) < 0.0005f)
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

	//Angular velocity
	{
		Vector3 aux1 = onePointOfContact.Cross(TangentOfImpulse);
		Vector3 aux2 = aux1.Transform(aux1, tensor1);
		one.m_newAngularVelocity = one.m_angularVelocity - aux2;
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
		if (m_isSleeping)
		{
			glColor3d(0, 1, 0);
		}
		else{
			glColor3d(1, 1, 0);
		}
		if(m_angularVelocity.Length() > 0)
		{
			glColor3d(0, 1, 1);
		}
		glBindTexture(GL_TEXTURE_2D, GetTexture());               // Select Our Texture, Maybe bad for performance
		GLUquadric *quadric = gluNewQuadric();
		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluQuadricTexture(quadric, GL_TRUE); 
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluSphere(quadric, m_radius-0.05f, 20, 20);
		
	glPopMatrix();
}


