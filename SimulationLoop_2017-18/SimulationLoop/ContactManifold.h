#pragma once
#include "Shape.h"

struct ManifoldPoint {
	Shape *contactID1;
	Shape *contactID2;
	Vector3 contactNormal;
	float penetration;
	Vector3 contactPoint;
};

class ContactManifold
{
public:
	ContactManifold(void);
	~ContactManifold(void);

	void Add(ManifoldPoint point);
	void Clear();
	int GetNumPoints() const;
	ManifoldPoint& GetPoint(int index);

private:
	ManifoldPoint m_points[1000];
	int m_numOfPoints;
};

