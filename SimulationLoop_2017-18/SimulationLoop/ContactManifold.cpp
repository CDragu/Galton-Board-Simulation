#include "ContactManifold.h"
#include <iostream>

ContactManifold::ContactManifold(void) : m_numOfPoints(0)
{
}


ContactManifold::~ContactManifold(void)
= default;

void ContactManifold::Add(ManifoldPoint point)
{
	if(m_numOfPoints >= 1000)
	{
		std::cout << "Too many collisions";
		return;
	}
	m_points[m_numOfPoints] = point;
	++m_numOfPoints;
}

void ContactManifold::Clear()
{
	m_numOfPoints = 0;
}

int ContactManifold::GetNumPoints() const
{
	return m_numOfPoints;
}

ManifoldPoint& ContactManifold::GetPoint(int index)
{
	return m_points[index];
}