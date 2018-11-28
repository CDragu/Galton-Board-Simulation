#pragma once

#include <Windows.h>
#include "Sphere.h"
#include "ContactManifold.h"
#include <list>
#include "PotentialContactManifold.h"
#include <AntTweakBar.h>
#include <vector>

class Game
{
public:
	Game(HDC hdc);
	~Game(void);

	void Update();
	void CreateBox(Vector3 origin, float height, float length, float width);
	void CreateBox(Vector3 origin, float height, float length, float width, Vector3 Rotation);
	void CreateCylinder(Vector3 origin);
	void ConstructBoard();
	void KeyboardResponse(const char key);
	void SimulationLoop();
	void CalculateObjectPhysics();
	void DynamicCollisionDetection();
	void DynamicCollisionResponse();
	void UpdateObjectPhysics();
	void CorrectObjectSinking();
	void Render();

	TwBar* Bar;
	HDC   m_hdc;
	float m_dt;
	int	  m_fps;
	float m_previousTime;
	std::vector<Shape*> ListOfShapes;
	
	//Experimental TODO: Remove if not working
	std::vector<Node*> ListOfNodes;
	
	Vector3 eye;
	Vector3 lookAt;
	ContactManifold *m_manifold;
	LARGE_INTEGER start, end, frequency;

	int ImpulseIteration;
	float TotalForce;
};

