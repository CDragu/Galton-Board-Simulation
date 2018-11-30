#include "Game.h"
#include <gl\gl.h>                                // Header File For The OpenGL32 Library from 1998
#include <gl\GLU.h>
#include <wincon.h>
#include <iostream>
#include <ctime>
#include "Cube.h"
#include "Cylinder.h"
#include <AntTweakBar.h>

Game::Game(HDC hdc) : m_hdc(hdc), m_previousTime(0), DepenetrationValue(0.85f), TimeStep(0.008f), SleepThreshold(0.05f), SizeOfBalls(2.0f)
{
	Paused = true;
	NumberOfBalls = 0;
	Elasticity = 0.8f;
	Friction = 0.7f;
	Bar = TwNewBar("Bar");
	TwDefine(("Bar  position='0 0' "));
	TwDefine(("Bar  size='300 150' "));
	TwDefine("Bar refresh=0.1 ");
	TwAddVarRW(Bar, "FPS", TW_TYPE_INT32, &m_fps, "");
	TwAddVarRW(Bar, "Number of Balls(1): ", TW_TYPE_FLOAT, &NumberOfBalls, "");
	TwAddVarRW(Bar, "Elasticity(O, L): ", TW_TYPE_FLOAT, &Elasticity, "");
	TwAddVarRW(Bar, "Friction(I, K): ", TW_TYPE_FLOAT, &Friction, "");
	TwAddVarRW(Bar, "Time Step(I, J): ", TW_TYPE_FLOAT, &TimeStep, "");
	TwAddVarRW(Bar, "Velocity Magnitude: ", TW_TYPE_FLOAT, &TotalForce, "");
	TwAddVarRW(Bar, "Size of Balls(T, B): ", TW_TYPE_FLOAT, &SizeOfBalls, "");
	
	//TwAddVarRW(Bar, "Depenetration Value: ", TW_TYPE_FLOAT, &DepenetrationValue, "");
	//TwAddVarRW(Bar, "Sleep Threshold: ", TW_TYPE_FLOAT, &SleepThreshold, "");
	ImpulseIteration = 6; // how many times we do the physics calculation for collision response, helps with jitter and objects not sinking

	m_manifold = new ContactManifold();

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	ConstructBoard();
	//InitializeTestEnviroment();
}

Game::~Game(void)
{
	ListOfShapes.clear();

	delete m_manifold;
}

void Game::Update()
{
	if(!Paused)
	{
		SimulationLoop();
	}
	
	Render();
}

void Game::InitializeTestEnviroment()
{
	eye = Vector3(0, 20, 200);
	for (size_t i = 0; i < 4; i++)
	{
		Sphere* m_sphere = new Sphere();
		m_sphere->SetPos(3, 20 + 40 * (i + 1), 20);
		m_sphere->SetRadius(SizeOfBalls);
		m_sphere->SetVel(0, -5, 0);
		m_sphere->SetMass(1.0f);
		m_sphere->SetVel(0, -30, 0);
		m_sphere->SetNewVel(Vector3(0.0f, 0.0f, 0.0f));
		m_sphere->SetRot(0, 0, 0);
		m_sphere->SetName("Sphere");
		m_sphere->GeometricType = 0;
		m_sphere->m_angularVelocity = Vector3(0, 0, 0);
		m_sphere->DefineInvTensor();
		m_sphere->m_sleepThreshold = SleepThreshold;
		m_sphere->m_restitution = Elasticity + 1;
		m_sphere->m_friction = Friction;
		ListOfShapes.push_back(m_sphere);
		NumberOfBalls++;
	}

	Cube* m_cube3 = new Cube();
	m_cube3->SetPos(-35.5f, 20, 0);
	m_cube3->SetName("Cube");
	m_cube3->SetSize(1);
	m_cube3->SetHeight(100);
	m_cube3->SetLength(5);
	m_cube3->SetWidth(50);
	m_cube3->SetRot(0, 0, 0);
	m_cube3->GeometricType = 1;
	m_cube3->CalculatePoints();
	m_cube3->DefineInvTensor();
	ListOfShapes.push_back(m_cube3);


	Cube* m_cube4 = new Cube();
	m_cube4->SetPos(-20.5f, -10, 0);
	m_cube4->SetName("Cube");
	m_cube4->SetSize(1);
	m_cube4->SetHeight(20);
	m_cube4->SetLength(5);
	m_cube4->SetWidth(50);
	m_cube4->SetRot(0, 0, 0);
	m_cube4->GeometricType = 1;
	m_cube4->CalculatePoints();
	m_cube4->DefineInvTensor();
	ListOfShapes.push_back(m_cube4);


	//Construct Floor;
	Cube* m_cube2 = new Cube();
	m_cube2->SetPos(0.2f, -20.0f, 0.3f);
	m_cube2->SetName("Cube");
	m_cube2->SetSize(1);
	m_cube2->SetHeight(6);
	m_cube2->SetLength(100);
	m_cube2->SetRot(0, 0, 0);
	m_cube2->SetWidth(100);
	//m_cube2->SetRot(-3.14f / 4.0f, 0, 0);
	m_cube2->SetRot(0, 0, 0);
	m_cube2->GeometricType = 1;
	m_cube2->m_mass = 20000;
	m_cube2->CalculatePoints();
	m_cube2->DefineInvTensor();
	ListOfShapes.push_back(m_cube2);
	{
		Cylinder* m_cylinder = new Cylinder();
		m_cylinder->SetPos(4, 10, 0);
		m_cylinder->SetName("Cylinder");
		m_cylinder->SetRadius(5.0f);
		m_cylinder->SetHeight(50.0f);
		m_cylinder->m_mass = 20000;
		m_cylinder->GeometricType = 2;
		ListOfShapes.push_back(m_cylinder);
	}

	{
		Cylinder* m_cylinder = new Cylinder();
		m_cylinder->SetPos(8, 30, 0);
		m_cylinder->SetName("Cylinder");
		m_cylinder->SetRadius(5.0f);
		m_cylinder->SetHeight(50.0f);
		m_cylinder->m_mass = 20000;
		m_cylinder->GeometricType = 2;
		ListOfShapes.push_back(m_cylinder);
	}
}

void Game::CreateBox(Vector3 origin, float height, float length, float width)
{
	float relativeSize = 10;

	Cube* bottom = new Cube();
	bottom->SetPos(origin * relativeSize);
	bottom->SetName("BoardBox");
	bottom->SetSize(1);
	bottom->SetHeight(height * relativeSize);
	bottom->SetLength(length * relativeSize);
	bottom->SetWidth(width * relativeSize);
	bottom->SetRot(0, 0, 0);
	bottom->GeometricType = 1;
	bottom->m_mass = 20000;
	bottom->CalculatePoints();
	bottom->DefineInvTensor();
	bottom->m_restitution = Elasticity + 1;
	bottom->m_friction = Friction;
	ListOfShapes.push_back(bottom);
}

void Game::CreateBox(Vector3 origin, float height, float length, float width, Vector3 Rotation)
{
	float relativeSize = 10;

	Cube* bottom = new Cube();
	bottom->SetPos(origin * relativeSize);
	bottom->SetName("BoardBox");
	bottom->SetSize(1);
	bottom->SetHeight(height * relativeSize);
	bottom->SetLength(length * relativeSize);
	bottom->SetWidth(width * relativeSize);
	bottom->SetRot(Rotation);
	bottom->GeometricType = 1;
	bottom->m_mass = 20000;
	bottom->CalculatePoints();
	bottom->DefineInvTensor();
	bottom->m_restitution = Elasticity + 1;
	bottom->m_friction = Friction;
	ListOfShapes.push_back(bottom);
}

void Game::CreateCylinder(Vector3 origin)
{
	float relativeSize = 10;

	Cylinder* m_cylinder = new Cylinder();
	m_cylinder->SetPos(origin * relativeSize);
	m_cylinder->SetName("Cylinder");
	m_cylinder->SetRadius(0.1f * relativeSize);
	m_cylinder->SetHeight(50.0f);
	m_cylinder->m_mass = 20000;
	m_cylinder->GeometricType = 2;
	m_cylinder->m_restitution = Elasticity + 1;
	m_cylinder->m_friction = Friction;
	ListOfShapes.push_back(m_cylinder);
}

void Game::ConstructBoard() {
	
	Vector3 origin(0, 20, 0);
	Vector3 originRotation(0, 0, 0);
	eye = Vector3(0, 400, 300);
	lookAt = Vector3(0, 400, 0);

	//Create Balls in right pos
	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			float relativeSize = 10;

			float xPos = (j * 1.3f) - 6.0f;
			float yPos = (i * 1) + 22;

			Sphere* m_sphere = new Sphere();
			m_sphere->SetPos((origin + Vector3(xPos, yPos, 2))* relativeSize);
			m_sphere->SetRadius(SizeOfBalls);
			m_sphere->SetMass(1.0f);
			m_sphere->SetName("Sphere");
			m_sphere->GeometricType = 0;
			m_sphere->m_sleepThreshold = SleepThreshold;
			m_sphere->m_restitution = Elasticity + 1;
			m_sphere->m_friction = Friction;
			m_sphere->DefineInvTensor();
			m_sphere->m_newPos = m_sphere->m_pos;
			ListOfShapes.push_back(m_sphere);
			NumberOfBalls++;
		}
	}

	//Create Bottom of box
	CreateBox(origin, 22, 12, 0.1f);

	//Create Right Side Of Box
	CreateBox(origin + Vector3(6, 0, 2), 22, 1.12f, 4);

	//Create Left Side Of Box
	CreateBox(origin + Vector3(-6, 0, 2), 22, 1.12f, 4);

	//Create Down side of box
	CreateBox(origin + Vector3(0, -13, 2), 4, 12, 4);

	//Create LeftSide of Top
	CreateBox(origin + Vector3(-3.5f, 11, 2), 1, 4, 5);

	//Create RightSide of Top
	CreateBox(origin + Vector3(+3.5f, 11, 2), 1, 4, 5);

	//Create Funnel back
	CreateBox(origin + Vector3(0, 16, 0), 10, 12, 0.1f);

	//Create Funnel Left
	CreateBox(origin + Vector3(-5.0f, 14.5f, 2), 10, 2, 4, Vector3(3.14f / 4.0f, 0, 0));

	//Create Funnel Right
	CreateBox(origin + Vector3(5.0f, 14.5f, 2), 10, 2, 4, Vector3(-3.14f / 4.0f, 0, 0));

	//Creating the bins
	for (int i = 0; i < 11; i++)
	{
		float xPos = (i * 0.95f) - 4.8f;
		CreateBox(origin + Vector3(xPos, -6, 2), 10, 0.0f, 4);
	}

	//Create the cylinder
	for (int i = 0; i < 10; i++)
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < 10; j++)
			{
				float xPos = (j * 1.05f) - 5.0f;
				float yPos = (i * 1.05f) ;
				CreateCylinder(origin + Vector3(xPos, yPos, 0));
			}
		}
		else
		{
			for (int j = 0; j < 10; j++)
			{
				float xPos = (j * 1.05f) - 4.5f;
				float yPos = (i * 1.05f) ;
				CreateCylinder(origin + Vector3(xPos, yPos, 0));
			}
		}
	}
}

void Game::Restart()
{
	for (int i = 0; i < ListOfShapes.size(); i++)
	{
		if((*ListOfShapes[i]).GeometricType == 0)
		{
			dynamic_cast<Sphere*>(ListOfShapes[i])->~Sphere();
		}

		if ((*ListOfShapes[i]).GeometricType == 1)
		{
			dynamic_cast<Cube*>(ListOfShapes[i])->~Cube();
		}

		if ((*ListOfShapes[i]).GeometricType == 2)
		{
			dynamic_cast<Cylinder*>(ListOfShapes[i])->~Cylinder();
		}
	}
	NumberOfBalls = 0;
	ListOfShapes.clear();

	//InitializeTestEnviroment();
	ConstructBoard();
}

void Game::AddBall()
{
	Sphere* m_sphere = new Sphere();
	m_sphere->SetPos(0, 350, 20);
	m_sphere->SetRadius(SizeOfBalls);
	m_sphere->SetMass(1.0f);
	m_sphere->SetName("Sphere");
	m_sphere->GeometricType = 0;
	m_sphere->m_sleepThreshold = SleepThreshold;
	m_sphere->DefineInvTensor();
	m_sphere->m_restitution = Elasticity + 1;
	m_sphere->m_friction = Friction;
	ListOfShapes.insert(ListOfShapes.begin(), m_sphere);
	NumberOfBalls++;
}

void Game::SimulationLoop()
{
	// calculate dt based on the simulation loop rate using a timer
	QueryPerformanceCounter(&end);
	m_dt = static_cast<float>((end.QuadPart - start.QuadPart) / static_cast<double>(frequency.QuadPart));
	start = end;

	m_fps = static_cast<int>(1.0 / m_dt);
	
	// Calculate the physic calculations on all objects (e.g. new position, velocity, etc)
	CalculateObjectPhysics();

	// Clear the manifold so that we can calculate all collisions for this simulation loop
	m_manifold->Clear();

	// Find dynamic collisions for all objects and add to contact manifold 
	CollisionDetection();

	// Handle dynamic collision responses using the contact manifold
	CollisionResponse();

	// Update the physics calculations on all objects (e.g. new position, velocity, etc)
	UpdateObjectPhysics();

	//Last step of physics simulation where we unstuck the objects that penetrated
	CorrectObjectSinking();
}


//**************************Update the physics calculations on each object***********************
void Game::CalculateObjectPhysics()
{
	//Updating the physics for the spheres, anything else dose not move so it's not important
	for (Shape* shape : ListOfShapes)
	{
		if(shape->GeometricType == 0)
		{
			dynamic_cast<Sphere*>(shape)->CalculatePhysics(TimeStep); 
		}else
		{
			return;
		}
		
	}
}

//**************************Handle collisions***********************
void Game::CollisionDetection()
{
	for(int i = 0; i <ListOfShapes.size(); i++)
	{
		if((*ListOfShapes[i]).GeometricType != 0)
		{
			continue;
		}
		for(int j = i+1; j < ListOfShapes.size(); j++)
		{
			const int geometricType = (*ListOfShapes[j]).GeometricType;
			if (geometricType == 0)
			{
				dynamic_cast<Sphere*>(ListOfShapes[i])->CollisionWithSphere(dynamic_cast<Sphere*>(ListOfShapes[j]), m_manifold);
			}

			if (geometricType == 1)
			{
				dynamic_cast<Sphere*>(ListOfShapes[i])->CollisionWithCubeWithAxisSeparation(dynamic_cast<Cube*>(ListOfShapes[j]), m_manifold);
			}

			if (geometricType == 2)
			{
				dynamic_cast<Sphere*>(ListOfShapes[i])->CollisionWithCylinder(dynamic_cast<Cylinder*>(ListOfShapes[j]), m_manifold);
			}
		}
	}
}

//**************************Handle collision responses***********************
void Game::CollisionResponse()
{
	for(int k = 0; k < ImpulseIteration; k++) 
		for(int collision = 0; collision < m_manifold->GetNumPoints(); ++collision)
		{
			ManifoldPoint &point = m_manifold->GetPoint(collision);

			if(point.contactID2->GeometricType == 0)
			{
				auto contact_id1 = dynamic_cast<Sphere*>(point.contactID1);
				contact_id1->CollisionResponseWithSphere(*contact_id1, *dynamic_cast<Sphere*>(point.contactID2), point.contactNormal, point.contactPoint);
				contact_id1->Update();
				dynamic_cast<Sphere*>(point.contactID2)->Update();
			}

			else if (point.contactID2->GeometricType == 1)
			{
				auto contact_id1 = dynamic_cast<Sphere*>(point.contactID1);
				contact_id1->CollisionResponseWithCube(*contact_id1, *dynamic_cast<Cube*>(point.contactID2), point.contactNormal, point.contactPoint);
				contact_id1->Update();
			}

			else 
			{
				auto contact_id1 = dynamic_cast<Sphere*>(point.contactID1);
				contact_id1->CollisionResponseWithCylinder(*contact_id1, *dynamic_cast<Cylinder*>(point.contactID2), point.contactNormal, point.contactPoint);//TODO: BROKEN
				contact_id1->Update();
			}
			
		}
}

//**************************Update the physics calculations on each object***********************
void Game::UpdateObjectPhysics()
{
	TotalForce = 0;
	for (auto& shape : ListOfShapes)
	{
		if (shape->GeometricType == 0)
		{
			Sphere* s = dynamic_cast<Sphere*>(shape);
			s->Update();
			TotalForce += s->m_newVelocity.LengthSquared();
		}else
		{
			return;
		}
	}
}

void Game::CorrectObjectSinking()
{
	for (int collision = 0; collision < m_manifold->GetNumPoints(); ++collision)
	{
		ManifoldPoint &point = m_manifold->GetPoint(collision);
		float depth = fmax(point.penetration + 0.02f, 0.0f);
		if (depth == 0.0f) { continue; }
		if(point.contactID2->GeometricType == 0)
		{
			if(dynamic_cast<Sphere*>(point.contactID1)->m_isSleeping)
			{
				continue;
			}

			float m1 = dynamic_cast<Sphere*>(point.contactID1)->InvertMass();
			float m2 = dynamic_cast<Sphere*>(point.contactID2)->InvertMass();
			float scalar = depth / (m1 + m2);

			Vector3 correction = point.contactNormal * scalar * DepenetrationValue;

			Vector3 CorrectionSphere1 = point.contactID1->GetPos() - correction * m1;
			point.contactID1->SetPos(CorrectionSphere1.x , CorrectionSphere1.y, CorrectionSphere1.z);
			
			//Vector3 CorrectionSphere2 = point.contactID2->GetPos() - correction * m2;
			//point.contactID2->SetPos(CorrectionSphere2.x, CorrectionSphere2.y, CorrectionSphere2.z); //TODO: Have another look at this, may be broken, maybe we need to keep this values with us until next update frame and make a sum
		}

		if(point.contactID2->GeometricType == 1)
		{
			if (dynamic_cast<Sphere*>(point.contactID1)->m_isSleeping)
			{
				continue;
			}

			float m1 = dynamic_cast<Sphere*>(point.contactID1)->InvertMass();
			float scalar = depth / (m1);

			Vector3 correction = point.contactNormal * scalar * DepenetrationValue;

			Vector3 CorrectionSphere1 = point.contactID1->GetPos() + correction * m1;
			point.contactID1->SetPos(CorrectionSphere1.x, CorrectionSphere1.y, CorrectionSphere1.z);

		}

		if (point.contactID2->GeometricType == 2)
		{
			if (dynamic_cast<Sphere*>(point.contactID1)->m_isSleeping)
			{
				continue;
			}

			float m1 = dynamic_cast<Sphere*>(point.contactID1)->InvertMass();
			float m2 = dynamic_cast<Cylinder*>(point.contactID2)->InvertMass();
			float scalar = depth / (m1 + m2);

			Vector3 correction = point.contactNormal * scalar * DepenetrationValue;//TODO:Linear projection percent, make visible variable

			Vector3 CorrectionSphere1 = point.contactID1->GetPos() + correction * m1;
			point.contactID1->SetPos(CorrectionSphere1.x, CorrectionSphere1.y, CorrectionSphere1.z);
		}
	}
}

//**************************Render and display the scene in OpenGL***********************
void Game::Render()									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	gluLookAt(
		eye.x, eye.y, eye.z,
		lookAt.x, lookAt.y, lookAt.z,
		0, 1, 0);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	for (auto& shape : ListOfShapes)
	{
		shape->Render();
	}

	if(TwDraw() == 0)
	{
		std::cout << TwGetLastError();
	}

	SwapBuffers(m_hdc);				// Swap Buffers (Double Buffering)

	
}

void Game::KeyboardResponse(const char key)
{
	std::cout << key << std::endl;

	if (key == '1')
	{
		AddBall();
	}
	if (key == 'R')
	{
		Restart();
	}
	if (key == 'P')
	{
		if(Paused)
		{
			Paused = false;
		}else
		{
			Paused = true;
		}
	}

	if (key == 'U')
	{
		TimeStep += 0.001f;
	}
	if (key == 'J')
	{
		TimeStep -= 0.001f;
	}

	if(key == 'I')
	{
		Friction += 0.01f;
	}
	if(key == 'K')
	{
		Friction -= 0.01f;
	}
	if(key == 'T')
	{
		SizeOfBalls += 0.01f;
	}
	if(key == 'B')
	{
		SizeOfBalls -= 0.01f;
	}
	if(key == 'O')
	{
		Elasticity += 0.01f;
	}
	if(key == 'L')
	{
		Elasticity -= 0.01f;
	}

	if(key == 'S')
	{
		eye.y -= 10;
		lookAt.y -= 10;
	}
	if (key == 'W')
	{
		eye.y += 10;
		lookAt.y += 10;
	}
	if (key == 'A')
	{
		eye.x -= 10;
		lookAt.x -= 10;
	}
	if (key == 'D')
	{
		eye.x += 10;
		lookAt.x += 10;
	}

	if (key == '&')
	{
		eye.z -= 10;
		lookAt.z -= 10;
	}
	if (key == '(')
	{
		eye.z += 10;
		lookAt.z += 10;
	}
	
	//if(key == 'L')
	//{
	//	DepenetrationValue -= 0.1f;
	//}
	//
	//if(key == 'O')
	//{
	//	SleepThreshold += 0.01f;
	//}
	//if(key == 'K')
	//{
	//	SleepThreshold -= 0.01f;
	//}
}



