#pragma once

#include <Windows.h>
#include "Sphere.h"
#include "ContactManifold.h"
#include <list>
#include "PotentialContactManifold.h"

class Game
{
public:
	Game(HDC hdc);
	~Game(void);

	void Update();
	void KeyboardResponse(const char key);
	void SimulationLoop();
	void CalculateObjectPhysics();
	void DynamicCollisionDetection();
	void DynamicCollisionResponse();
	void UpdateObjectPhysics();
	void Render();

	HDC   m_hdc;
	float m_dt;
	int	  m_fps;
	float m_previousTime;
	std::list<Shape*> ListOfShapes;
	
	//Experimental TODO: Remove if not working
	std::list<Node*> ListOfNodes;
	
	Vector3 eye;
	ContactManifold *m_manifold;
	LARGE_INTEGER start, end, frequency;
};

