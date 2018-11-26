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
	eye = Vector3(0, 0, 200);
	/*Node* ParentNode = new Node(nullptr, BoundingSphere(Vector3(0, 0, 0), 1.0f), nullptr);
	ListOfNodes.push_back(ParentNode);*/
	for (size_t i = 0; i < 1; i++)
	{
		Sphere* m_sphere = new Sphere();
		m_sphere->SetPos(0.2, 50 * (i+1), 0.3);
		m_sphere->SetRadius(3.0f);
		m_sphere->SetVel(0, -5, 0);
		m_sphere->SetMass(1.0f);
		m_sphere->SetVel(0, -30, 0);
		m_sphere->SetNewVel(Vector3(0.0f,0.0f, 0.0f));
		m_sphere->SetRot(0, 0, 0); 
		m_sphere->SetName("Sphere");
		m_sphere->GeometricType = 0;
		m_sphere->m_angularVelocity = Vector3(0, 0, 9);
		m_sphere->DefineInvTensor();
		ListOfShapes.push_back(m_sphere);
		//Experimental TODO: Remove if not working
		//ParentNode->Insert(m_sphere, BoundingSphere(m_sphere->GetPos(), m_sphere->GetRadius()));
		//Node* node = new Node(ParentNode, , m_sphere);
		//ListOfNodes.push_back(node);
	}

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

	//Cube* m_cube3 = new Cube();
	//m_cube3->SetPos(30, -20, 0);
	//m_cube3->SetName("Cube");
	//m_cube3->SetSize(1);
	//m_cube3->SetHeight(3);
	//m_cube3->SetLength(50);
	//m_cube3->SetRot(3.14f / 4.0f, 0, 0);
	//m_cube3->SetWidth(50);
	//m_cube3->GeometricType = 1;
	//m_cube3->CalculatePoints();
	//m_cube3->DefineInvTensor();
	//ListOfShapes.push_back(m_cube3);


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

	Cylinder* m_cylinder = new Cylinder();
	m_cylinder->SetPos(20, 10, 0);
	m_cylinder->SetName("Cylinder");
	m_cylinder->SetRadius(4.0f);
	m_cylinder->SetHeight(50.0f);
	ListOfShapes.push_back(m_cylinder);

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


	m_manifold = new ContactManifold();

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
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
	CorrectObjectSinking();
}


//**************************Update the physics calculations on each object***********************
void Game::CalculateObjectPhysics()
{
	for (Shape* shape : ListOfShapes)
	{
		if(shape->GetName() == "Sphere")
		{
			dynamic_cast<Sphere*>(shape)->CalculatePhysics(0.016f);
		}
		
	}
}

//**************************Handle dynamic collisions***********************
void Game::DynamicCollisionDetection()
{
	//TODO: remove if node ended up not working
	/*for (int i = 0; i < ListOfNodes.size(); i++)
	{
		PotentialContact* c = new PotentialContact();
		if((*ListOfNodes[i]).getPotentialContacts(c, 1000) == 0)
		{
			continue;
		}
		if(c->body[0]->GeometricType == 0 && c->body[1]->GeometricType == 0)
		{
			dynamic_cast<Sphere*>(c->body[0])->CollisionWithSphere(dynamic_cast<Sphere*>(c->body[1]), m_manifold);
		}

		if (c->body[0]->GeometricType == 0 && c->body[1]->GeometricType == 1)
		{
			dynamic_cast<Sphere*>(c->body[0])->CollisionWithCubeWithAxisSeparation(dynamic_cast<Cube*>(c->body[1]), m_manifold);
		}
	}*/

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
		}
	}


	//std::list<Shape*>::iterator it1;
	//for (it1 = ListOfShapes.begin(); it1 != ListOfShapes.end(); it1++)
	//{
	//	auto it2 = it1;
	//	for(++it2; it2 != ListOfShapes.end(); it2++)
	//	{
	//		if (true)//((*it1)->GetName() == "Sphere" && (*it2)->GetName() == "Sphere")
	//		{
	//			//dynamic_cast<Sphere*>(*it1)->CollisionWithSphere(dynamic_cast<Sphere*>(*it2), m_manifold);
	//		}

	//		if (false)//((*it1)->GetName() == "Sphere" && (*it2)->GetName() == "Cube")
	//		{
	//			//dynamic_cast<Sphere*>(*it1)->CollisionWithCubeWithAxisSeparation(dynamic_cast<Cube*>(*it2), m_manifold);
	//		}
	//	}
	//}
}

//**************************Handle dynamic collision responses***********************
void Game::DynamicCollisionResponse()
{
	for(int k = 0; k < ImpulseIteration; k++)
		for(int collision = 0; collision < m_manifold->GetNumPoints(); ++collision)
		{
			ManifoldPoint &point = m_manifold->GetPoint(collision);

			if(point.contactID1->GetName() == "Sphere" && point.contactID2->GetName() == "Sphere")
			{
				dynamic_cast<Sphere*>(point.contactID1)->CollisionResponseWithSphere(*dynamic_cast<Sphere*>(point.contactID1), *dynamic_cast<Sphere*>(point.contactID2), point.contactNormal, point.contactPoint);
				dynamic_cast<Sphere*>(point.contactID1)->Update();
				dynamic_cast<Sphere*>(point.contactID2)->Update();
			}

			if (point.contactID1->GetName() == "Sphere" && point.contactID2->GetName() == "Cube")
			{
				dynamic_cast<Sphere*>(point.contactID1)->CollisionResponseWithCube(*dynamic_cast<Sphere*>(point.contactID1), *dynamic_cast<Cube*>(point.contactID2), point.contactNormal, point.contactPoint);
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

		float depth = fmax(point.penetration - 0.02, 0.0f);
		if(point.contactID1->GeometricType == 0 && point.contactID2->GeometricType == 0)
		{
			float m1 = dynamic_cast<Sphere*>(point.contactID1)->InvertMass();
			float m2 = dynamic_cast<Sphere*>(point.contactID2)->InvertMass();
			float scalar = depth / (m1 + m2);

			Vector3 correction = point.contactNormal * scalar * 0.4f;//TODO:Linear projection percent, make visible variable

			Vector3 CorrectionSphere1 = point.contactID1->GetPos() - correction * m1;
			point.contactID1->SetPos(CorrectionSphere1.x , CorrectionSphere1.y, CorrectionSphere1.z);
			
			Vector3 CorrectionSphere2 = point.contactID2->GetPos() - correction * m2;
			point.contactID2->SetPos(CorrectionSphere2.x, CorrectionSphere2.y, CorrectionSphere2.z);
		}

		if(point.contactID1->GeometricType == 0 && point.contactID2->GeometricType == 1)
		{
			float m1 = dynamic_cast<Sphere*>(point.contactID1)->InvertMass();
			float scalar = depth / (m1);

			Vector3 correction = point.contactNormal * scalar * 0.4f;//TODO:Linear projection percent, make visible variable

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
	gluLookAt(eye.x, eye.y, eye.z, 0, 0, 0, 0, 1, 0);

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



