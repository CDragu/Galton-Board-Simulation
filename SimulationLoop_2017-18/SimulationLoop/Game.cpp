#include "Game.h"
#include <gl\gl.h>                                // Header File For The OpenGL32 Library
#include <gl\GLU.h>
#include <wincon.h>
#include <iostream>
#include <ctime>
#include "Cube.h"
#include "Cylinder.h"



Game::Game(HDC hdc) : m_hdc(hdc), m_previousTime(0)
{
	for (size_t i = 0; i < 200; i++)
	{
		Sphere* m_sphere = new Sphere();
		m_sphere->SetPos(0.2, 30 * (i+1), 0.3);
		m_sphere->SetRadius(1.0f);
		m_sphere->SetVel(0, -5, 0);
		m_sphere->SetMass(750.0f);
		m_sphere->SetVel(0, 0, 0);
		m_sphere->SetNewVel(Vector3(0.0f, 0.0f, 0.0f));
		m_sphere->SetRot(0, 0, 0);
		m_sphere->SetName("Sphere");
		ListOfShapes.push_back(m_sphere);
		//Experimental TODO: Remove if not working
		Node* node = new Node(nullptr, BoundingSphere(m_sphere->GetPos(), m_sphere->GetRadius()), m_sphere);
		ListOfNodes.push_back(node);
	}

	
	Cube* m_cube = new Cube();
	m_cube->SetPos(0, -30, 30);
	m_cube->SetName("Cube");
	m_cube->SetSize(1);
	m_cube->SetHeight(3);
	m_cube->SetLength(100); 
	m_cube->SetRot(3.14f/4.0f, -3.14f / 4.0f, 0);
	m_cube->SetWidth(50);
	
	ListOfShapes.push_back(m_cube);

	/*Cube* m_cube3 = new Cube();
	m_cube3->SetPos(-10, 0, 0);
	m_cube3->SetName("Cube");
	m_cube3->SetSize(1);
	m_cube3->SetHeight(3);
	m_cube3->SetLength(50);
	m_cube3->SetRot(-3.14f / 4.0f, 0, 0);
	m_cube3->SetWidth(50);

	ListOfShapes.push_back(m_cube3);*/


	//Construct Floor;
	Cube* m_cube2 = new Cube();
	m_cube2->SetPos(0, -41.5f, 0);
	m_cube2->SetName("Cube");
	m_cube2->SetSize(1);
	m_cube2->SetHeight(3);
	m_cube2->SetLength(100);
	m_cube2->SetRot(0, 0, 0);
	m_cube2->SetWidth(100);

	ListOfShapes.push_back(m_cube2);

	Cylinder* m_cylinder = new Cylinder();
	m_cylinder->SetPos(20, 10, 0);
	m_cylinder->SetName("Cylinder");
	m_cylinder->SetRadius(4.0f);
	m_cylinder->SetHeight(50.0f);
	ListOfShapes.push_back(m_cylinder);


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
	//DynamicCollisionDetection();

	// Handle dynamic collision responses using the contact manifold
	DynamicCollisionResponse();

	// Update the physics calculations on all objects (e.g. new position, velocity, etc)
	UpdateObjectPhysics();
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

	std::list<Shape*>::iterator it1;
	for (it1 = ListOfShapes.begin(); it1 != ListOfShapes.end(); it1++)
	{
		auto it2 = it1;
		for(++it2; it2 != ListOfShapes.end(); it2++)
		{
			if ((*it1)->GetName() == "Sphere" && (*it2)->GetName() == "Sphere")
			{
				dynamic_cast<Sphere*>(*it1)->CollisionWithSphere(dynamic_cast<Sphere*>(*it2), m_manifold);
			}

			if ((*it1)->GetName() == "Sphere" && (*it2)->GetName() == "Cube")
			{
				dynamic_cast<Sphere*>(*it1)->CollisionWithCube(dynamic_cast<Cube*>(*it2), m_manifold);
			}
		}
	}
}

//**************************Handle dynamic collision responses***********************
void Game::DynamicCollisionResponse()
{
	for(int collision = 0; collision < m_manifold->GetNumPoints(); ++collision)
	{
		ManifoldPoint &point = m_manifold->GetPoint(collision);

		if(point.contactID1->GetName() == "Sphere" && point.contactID2->GetName() == "Sphere")
		{
			dynamic_cast<Sphere*>(point.contactID1)->CollisionResponseWithSphere(*dynamic_cast<Sphere*>(point.contactID1), *dynamic_cast<Sphere*>(point.contactID2), point.contactNormal);
		}

		if (point.contactID1->GetName() == "Sphere" && point.contactID2->GetName() == "Cube")
		{
			dynamic_cast<Sphere*>(point.contactID1)->CollisionResponseWithCube(*dynamic_cast<Sphere*>(point.contactID1), *dynamic_cast<Cube*>(point.contactID2), point.contactNormal);
		}
		
	}
}

//**************************Update the physics calculations on each object***********************
void Game::UpdateObjectPhysics()
{
	for (auto& shape : ListOfShapes)
	{
		if (shape->GetName() == "Sphere")
		{
			dynamic_cast<Sphere*>(shape)->Update();
		}
	}
}

//**************************Render and display the scene in OpenGL***********************
void Game::Render()									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	gluLookAt(0, 0, 200, 0, 0, 0, 0, 1, 0);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	for (auto& shape : ListOfShapes)
	{
		shape->Render();
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
}



