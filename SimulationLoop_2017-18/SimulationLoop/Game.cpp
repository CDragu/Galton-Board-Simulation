#include "Game.h"
#include <gl\gl.h>                                // Header File For The OpenGL32 Library
#include <gl\GLU.h>
#include <wincon.h>

Game::Game(HDC hdc) : m_hdc(hdc), m_previousTime(0)
{
	for (size_t i = 0; i < 4; i++)
	{
		Sphere *m_sphere;
		m_sphere = new Sphere();
		m_sphere->SetName(std::to_string(i));
		m_sphere->SetPos(0, 15 * i+1);
		m_sphere->SetVel(0, -5);
		m_sphere->SetMass(750.0f);
		m_sphere->SetVel(0, 0);
		m_sphere->SetNewVel(Vector2f(0.0f, 0.0f));

		ListOfSpheres.push_back(*m_sphere);
	}

	m_manifold = new ContactManifold();

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
}

Game::~Game(void)
{
	/*for (auto& sphere : ListOfSpheres)
	{
		delete &sphere;
	}*/

	ListOfSpheres.clear();

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
}


//**************************Update the physics calculations on each object***********************
void Game::CalculateObjectPhysics()
{
	for (auto& sphere : ListOfSpheres)
	{
		sphere.CalculatePhysics(m_dt);
	}
}

//**************************Handle dynamic collisions***********************
void Game::DynamicCollisionDetection()
{
	for (int i = 0; i <= ListOfSpheres.size(); i++)
	{
		for (int j = i+1; j < ListOfSpheres.size(); j++)
		{
			Sphere& it1 = *std::next(ListOfSpheres.begin(), i);
			Sphere& it2 = *std::next(ListOfSpheres.begin(), j);

			//std::next(ListOfSpheres.begin(), i)->CollisionWithSphere(&*std::next(ListOfSpheres.begin(), j), m_manifold);
			it1.CollisionWithSphere(&it2, m_manifold);
		}
	}
}

//**************************Handle dynamic collision responses***********************
void Game::DynamicCollisionResponse()
{
	for(int collision = 0; collision < m_manifold->GetNumPoints(); ++collision)
	{
		ManifoldPoint &point = m_manifold->GetPoint(collision);
		point.contactID1->CollisionResponseWithSphere(point);
	}
}

//**************************Update the physics calculations on each object***********************
void Game::UpdateObjectPhysics()
{
	for (auto& sphere : ListOfSpheres)
	{
		sphere.Update();
	}
}

//**************************Render and display the scene in OpenGL***********************
void Game::Render()									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	gluLookAt(0, 50, 100, 0, 0, 0, 0, 1, 0);

	glDisable(GL_TEXTURE_2D);
	// Draw plane (at y=-20)
	glBegin(GL_QUADS);
		glColor3d(1, 1, 1);
		glVertex3d(-50, -20, -50);
		glVertex3d( 50, -20, -50);
		glVertex3d( 50, -20,  50);
		glVertex3d(-50, -20,  50);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	for (auto& sphere : ListOfSpheres)
	{
		sphere.Render();
	}

	SwapBuffers(m_hdc);				// Swap Buffers (Double Buffering)
}

