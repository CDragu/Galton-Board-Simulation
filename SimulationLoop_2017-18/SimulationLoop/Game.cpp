#include "Game.h"
#include <gl\gl.h>                                // Header File For The OpenGL32 Library
#include <gl\GLU.h>
#include <wincon.h>
#include <iostream>
#include <ctime>
#include "Cube.h"
#include "Cylinder.h"
#include <AntTweakBar.h>

Game::Game(HDC hdc) : m_hdc(hdc), m_previousTime(0)
{
	Bar = TwNewBar("Bar");
	TwDefine(("Bar  position='0 0' "));
	TwDefine(("Bar  size='200 50' "));
	TwDefine("Bar refresh=0.1 ");
	TwAddVarRW(Bar, "FPS", TW_TYPE_INT32, &m_fps, "");
	TwAddVarRW(Bar, "Velocity Sum: ", TW_TYPE_FLOAT, &TotalForce, "");

	ImpulseIteration = 6; // how many times we do the physics calculation for collision response
	eye = Vector3(0, 20, 200);
	/*Node* ParentNode = new Node(nullptr, BoundingSphere(Vector3(0, 0, 0), 1.0f), nullptr);
	ListOfNodes.push_back(ParentNode);*/
	for (size_t i = 0; i < 0; i++)
	{
		Sphere* m_sphere = new Sphere();
		m_sphere->SetPos(3, 20 + 40 * (i+1), 20);
		m_sphere->SetRadius(3.0f);
		m_sphere->SetVel(0, -5, 0);
		m_sphere->SetMass(1.0f);
		m_sphere->SetVel(0, -30, 0);
		m_sphere->SetNewVel(Vector3(0.0f,0.0f, 0.0f));
		m_sphere->SetRot(0, 0, 0); 
		m_sphere->SetName("Sphere");
		m_sphere->GeometricType = 0;
		m_sphere->m_angularVelocity = Vector3(0, 0, 0);
		m_sphere->m_friction = 2;
		m_sphere->DefineInvTensor();
		ListOfShapes.push_back(m_sphere);
		//Experimental TODO: Remove if not working
		//ParentNode->Insert(m_sphere, BoundingSphere(m_sphere->GetPos(), m_sphere->GetRadius()));
		//Node* node = new Node(ParentNode, , m_sphere);
		//ListOfNodes.push_back(node);
	}

	ConstructBoard();

	m_manifold = new ContactManifold();

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	return;

	//
	//Cube* m_cube = new Cube();
	//m_cube->SetPos(0, -30, 0.3);
	//m_cube->SetName("Cube");
	//m_cube->SetSize(1);
	//m_cube->SetHeight(3);
	//m_cube->SetLength(100); 
	//m_cube->SetRot(3.14f/4.0f, -3.14f / 4.0f, 3.14f / 4.0f);
	////m_cube->SetRot(0, 0, 0.383f);
	//m_cube->SetWidth(50);
	//m_cube->CreateTransformMatrix();
	//m_cube->GeometricType = 1;
	//ListOfShapes.push_back(m_cube);

	/*Cube* m_cube3 = new Cube();
	m_cube3->SetPos(-1.5f, 20, 0);
	m_cube3->SetName("Cube");
	m_cube3->SetSize(1);
	m_cube3->SetHeight(5);
	m_cube3->SetLength(5);
	m_cube3->SetWidth(5);
	m_cube3->SetRot(0, 0, 0);
	m_cube3->GeometricType = 1;
	m_cube3->CalculatePoints();
	m_cube3->DefineInvTensor();
	ListOfShapes.push_back(m_cube3);*/


	//Construct Floor;
	Cube* m_cube2 = new Cube();
	m_cube2->SetPos(0.2f, -20.0f, 0.3f);
	m_cube2->SetName("Cube");
	m_cube2->SetSize(1);
	m_cube2->SetHeight(6);
	m_cube2->SetLength(100);
	m_cube2->SetRot(0,0 , 0);
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

	
	//Sphere* m_sphere = new Sphere();
	//m_sphere->SetPos(0.2, 10, 0.3);
	//m_sphere->SetRadius(3.0f);
	//m_sphere->SetMass(750000.0f);
	//m_sphere->SetVel(0, 0, 0);
	//m_sphere->SetNewVel(Vector3(0.0f, 0.0f, 0.0f));
	//m_sphere->SetRot(0, 3.14f / 4.0f, 0); 
	//m_sphere->SetName("Sphere");
	//m_sphere->GeometricType = 0;
	////m_sphere->m_angularVelocity = Vector3(90, 0, 0);
	//m_sphere->DefineInvTensor();
	//ListOfShapes.push_back(m_sphere);
}

Game::~Game(void)
{
	ListOfShapes.clear();

	delete m_manifold;
}

void Game::Update()
{
	// **************************************************
	// The simulation loop should be on its own thread(s)
	// **************************************************
	SimulationLoop();
	
	Render();
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
	ListOfShapes.push_back(bottom);
}

void Game::CreateCylinder(Vector3 origin)
{
	float relativeSize = 10;

	Cylinder* m_cylinder = new Cylinder();
	m_cylinder->SetPos(origin * relativeSize);
	m_cylinder->SetName("Cylinder");
	m_cylinder->SetRadius(1.5f);
	m_cylinder->SetHeight(50.0f);
	m_cylinder->m_mass = 20000;
	m_cylinder->GeometricType = 2;
	ListOfShapes.push_back(m_cylinder);
}

void Game::ConstructBoard() {
	
	Vector3 origin(0, 20, 0);
	Vector3 originRotation(0, 0, 0);
	eye = Vector3(0, 400, 300);
	lookAt = Vector3(0, 200, 0);

	//Create Balls in right pos
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			float relativeSize = 10;

			float xPos = (j * 1.3f) - 6.0f;
			float yPos = (i * 1) + 22;

			Sphere* m_sphere = new Sphere();
			m_sphere->SetPos((origin + Vector3(xPos, yPos, 2))* relativeSize);
			m_sphere->SetRadius(2.0f);
			m_sphere->SetMass(1.0f);
			//m_sphere->SetNewVel(Vector3(0.0f, 0.0f, 0.0f));
			//m_sphere->SetRot(0, 0, 0);
			m_sphere->SetName("Sphere");
			m_sphere->GeometricType = 0;
			//m_sphere->m_angularVelocity = Vector3(0, 0, 3);
			m_sphere->m_friction = 2;
			m_sphere->DefineInvTensor();
			ListOfShapes.push_back(m_sphere);
		}
	}

	//Create Bottom of box
	CreateBox(origin, 22, 12, 0.1f);

	//Create Right Side Of Box
	CreateBox(origin + Vector3(6, 0, 2), 22, 1, 4);

	//Create Left Side Of Box
	CreateBox(origin + Vector3(-6, 0, 2), 22, 1, 4);

	//Create Down side of box
	CreateBox(origin + Vector3(0, -11, 2), 1, 12, 4);

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
	for (int i = 0; i < 8; i++)
	{
		float xPos = (i * 1.3f) - 4.5f;
		CreateBox(origin + Vector3(xPos, -6, 2), 10, 0.3f, 4);
	}

	//Create the cylinder
	for (int i = 0; i < 10; i++)
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < 8; j++)
			{
				float xPos = (j * 1.3f) - 4.5f;
				float yPos = (i * 1) - 1;
				CreateCylinder(origin + Vector3(xPos, yPos, 0));
			}
		}
		else
		{
			for (int j = 0; j < 7; j++)
			{
				float xPos = (j * 1.3f) - 4.0f;
				float yPos = (i * 1) - 1;
				CreateCylinder(origin + Vector3(xPos, yPos, 0));
			}
		}
	}
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
	DynamicCollisionDetection();

	// Handle dynamic collision responses using the contact manifold
	DynamicCollisionResponse();

	// Update the physics calculations on all objects (e.g. new position, velocity, etc)
	UpdateObjectPhysics();

	//Last step of physics simulation where we unstuck the objects that penetrated
	//CorrectObjectSinking();
}


//**************************Update the physics calculations on each object***********************
void Game::CalculateObjectPhysics()
{
	for (Shape* shape : ListOfShapes)
	{
		if(shape->GetName() == "Sphere")
		{
			dynamic_cast<Sphere*>(shape)->CalculatePhysics(0.016f); //TODO, variable
		}
		
	}
}

//**************************Handle dynamic collisions***********************
void Game::DynamicCollisionDetection()
{
	for(int i = 0; i <ListOfShapes.size(); i++)
	{
		for(int j = i+1; j < ListOfShapes.size(); j++)
		{
			if ((*ListOfShapes[i]).GeometricType == 0 && (*ListOfShapes[j]).GeometricType == 0)
			{
				dynamic_cast<Sphere*>(ListOfShapes[i])->CollisionWithSphere(dynamic_cast<Sphere*>(ListOfShapes[j]), m_manifold);
			}

			if ((*ListOfShapes[i]).GeometricType == 0 && (*ListOfShapes[j]).GeometricType == 1)
			{
				dynamic_cast<Sphere*>(ListOfShapes[i])->CollisionWithCubeWithAxisSeparation(dynamic_cast<Cube*>(ListOfShapes[j]), m_manifold);
			}

			if ((*ListOfShapes[i]).GeometricType == 0 && (*ListOfShapes[j]).GeometricType == 2)
			{
				dynamic_cast<Sphere*>(ListOfShapes[i])->CollisionWithCylinder(dynamic_cast<Cylinder*>(ListOfShapes[j]), m_manifold);
			}
		}
	}
}

//**************************Handle dynamic collision responses***********************
void Game::DynamicCollisionResponse()
{
	for(int k = 0; k < ImpulseIteration; k++)
		for(int collision = 0; collision < m_manifold->GetNumPoints(); ++collision)
		{
			ManifoldPoint &point = m_manifold->GetPoint(collision);

			if(point.contactID1->GeometricType == 0 && point.contactID2->GeometricType == 0)
			{
				dynamic_cast<Sphere*>(point.contactID1)->CollisionResponseWithSphere(*dynamic_cast<Sphere*>(point.contactID1), *dynamic_cast<Sphere*>(point.contactID2), point.contactNormal, point.contactPoint);
				dynamic_cast<Sphere*>(point.contactID1)->Update();
				dynamic_cast<Sphere*>(point.contactID2)->Update();
			}

			if (point.contactID1->GeometricType == 0 && point.contactID2->GeometricType == 1)
			{
				dynamic_cast<Sphere*>(point.contactID1)->CollisionResponseWithCube(*dynamic_cast<Sphere*>(point.contactID1), *dynamic_cast<Cube*>(point.contactID2), point.contactNormal, point.contactPoint);
				dynamic_cast<Sphere*>(point.contactID1)->Update();
			}

			if (point.contactID1->GeometricType == 0 && point.contactID2->GeometricType == 2)
			{
				dynamic_cast<Sphere*>(point.contactID1)->CollisionResponseWithCylinder(*dynamic_cast<Sphere*>(point.contactID1), *dynamic_cast<Cylinder*>(point.contactID2), point.contactNormal, point.contactPoint);//TODO: BROKEN
				dynamic_cast<Sphere*>(point.contactID1)->Update();
			}
			
		}
}

//**************************Update the physics calculations on each object***********************
void Game::UpdateObjectPhysics()
{
	TotalForce = 0;
	for (auto& shape : ListOfShapes)
	{
		if (shape->GetName() == "Sphere")
		{
			Sphere* s = dynamic_cast<Sphere*>(shape);
			s->Update();
			TotalForce += s->m_newVelocity.LengthSquared();
		}
	}
}

void Game::CorrectObjectSinking()
{
	for (int collision = 0; collision < m_manifold->GetNumPoints(); ++collision)
	{
		ManifoldPoint &point = m_manifold->GetPoint(collision);

		float depth = fmax(point.penetration, 0.0f);
		if(point.contactID1->GeometricType == 0 && point.contactID2->GeometricType == 0)
		{
			float m1 = dynamic_cast<Sphere*>(point.contactID1)->InvertMass();
			float m2 = dynamic_cast<Sphere*>(point.contactID2)->InvertMass();
			float scalar = depth / (m1 + m2);

			Vector3 correction = point.contactNormal * scalar * 0.4f;//TODO:Linear projection percent, make visible variable

			Vector3 CorrectionSphere1 = point.contactID1->GetPos() - correction * m1;
			point.contactID1->SetPos(CorrectionSphere1.x , CorrectionSphere1.y, CorrectionSphere1.z);
			
			/*Vector3 CorrectionSphere2 = point.contactID2->GetPos() - correction * m2;
			point.contactID2->SetPos(CorrectionSphere2.x, CorrectionSphere2.y, CorrectionSphere2.z);*/ //TODO: Have another look at this, may be broken, maybe we need to keep this values with us until next update frame and make a sum
		}

		if(point.contactID1->GeometricType == 0 && point.contactID2->GeometricType == 1)
		{
			float m1 = dynamic_cast<Sphere*>(point.contactID1)->InvertMass();
			float scalar = depth / (m1);

			Vector3 correction = point.contactNormal * scalar * 0.4f;//TODO:Linear projection percent, make visible variable

			Vector3 CorrectionSphere1 = point.contactID1->GetPos() + correction * m1;
			point.contactID1->SetPos(CorrectionSphere1.x, CorrectionSphere1.y, CorrectionSphere1.z);

		}

		//if (point.contactID1->GeometricType == 0 && point.contactID2->GeometricType == 2)
		//{
		//	float m1 = dynamic_cast<Sphere*>(point.contactID1)->InvertMass();
		//	float m2 = dynamic_cast<Cylinder*>(point.contactID2)->InvertMass();
		//	float scalar = depth / (m1 + m2);

		//	Vector3 correction = point.contactNormal * scalar * 0.4f;//TODO:Linear projection percent, make visible variable

		//	Vector3 CorrectionSphere1 = point.contactID1->GetPos() - correction * m1;
		//	point.contactID1->SetPos(CorrectionSphere1.x, CorrectionSphere1.y, CorrectionSphere1.z);

		//	/*Vector3 CorrectionSphere2 = point.contactID2->GetPos() - correction * m2;
		//	point.contactID2->SetPos(CorrectionSphere2.x, CorrectionSphere2.y, CorrectionSphere2.z);*/ //TODO: Have another look at this, may be broken, maybe we need to keep this values with us until next update frame and make a sum
		//}
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

	if(key == 'U')
	{
		SimulationLoop();
	}

	if(key == 'W')
	{
		eye.z -= 10;
	}
	if (key == 'S')
	{
		eye.z += 10;
	}
	if (key == 'A')
	{
		eye.x -= 10;
	}
	if (key == 'D')
	{
		eye.x += 10;
	}
}



