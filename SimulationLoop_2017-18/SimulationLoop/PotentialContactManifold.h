#pragma once

#include "Shape.h"


struct PotentialContact
{
	Shape* body[2];
};

struct BoundingSphere
{
	BoundingSphere() = default;
	BoundingSphere(const Vector3& center, float radius);
	BoundingSphere(const BoundingSphere& one, const BoundingSphere& two);
	bool overlaps(const BoundingSphere* other) const;

	Vector3 pos;
	float radius;
};

class Node
{
public:
	Node(Node* node, BoundingSphere volume, Shape* p_body);
	~Node();

	Node* parent;
	Node* children[2];
	//Bounding volume

	BoundingSphere bounds;

	//Object in node if node is last in tree
	Shape* body;

	bool overlaps(const Node* other) const;

	bool isLeaf() const
	{
		return !(body != nullptr);
	}

	int getPotentialContacts(PotentialContact* contacts, int limit) const;
	int getPotentialContactsWith(const Node* other, PotentialContact* contacts, int limit) const;

	void Insert(Shape* body, const BoundingSphere &newVolume);

};



