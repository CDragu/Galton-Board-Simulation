#include "PotentialContactManifold.h"

bool Node::overlaps(const Node* other) const
{
	return this->overlaps(other);
}

int Node::getPotentialContacts(PotentialContact* contacts, int limit) const
{
	if (isLeaf() || limit == 0) return 0;

	return children[0]->getPotentialContactsWith(children[1], contacts, limit);
}

int Node::getPotentialContactsWith(const Node* other, PotentialContact* contacts, int limit) const
{
	if (!overlaps(other) || limit == 0) return 0;

	if (isLeaf() && other->isLeaf())
	{
		contacts->body[0] = body;
		contacts->body[1] = other->body;
		return 1;
	}

	if (other->isLeaf() ||
		(!isLeaf() && this->bounds.radius >= other->bounds.radius))
	{
		// Recurse into ourself.
		unsigned count = children[0]->getPotentialContactsWith(
			other, contacts, limit
		);
		// Check whether we have enough slots to do the other side too.
		if (limit > count) {
			return count + children[1]->getPotentialContactsWith(
				other, contacts + count, limit - count
			);
		}
		else {
			return count;
		}
	}
	else
	{
		// Recurse into the other node.
		unsigned count = getPotentialContactsWith(
			other->children[0], contacts, limit
		);
		// Check whether we have enough slots to do the other side too.
		if (limit > count) {
			return count + getPotentialContactsWith(
				other->children[1], contacts + count, limit - count
			);
		}
		else {
			return count;
		}
	}
}

void Node::Insert(Shape* newBody, const BoundingSphere& newVolume)
{
	// If we are a leaf, then the only option is to spawn two
// new children and place the new body in one.
	if (isLeaf())
	{
		// Child one is a copy of us.
		children[0] = new Node(
			this, bounds, body
			);
		// Child two holds the new body
		children[1] = new Node(
			this, newVolume, newBody
			);
		// And we now loosen the body (we’re no longer a leaf).
		this->body = NULL;
		// We need to recalculate our bounding volume.
		bounds = BoundingSphere(bounds, newVolume);
	}
	// Otherwise we need to work out which child gets to keep
	// the inserted body. We give it to whoever would grow the
	// least to incorporate it.
	else
	{
		if (BoundingSphere(children[0]->bounds, newVolume).radius <
			BoundingSphere(children[1]->bounds, newVolume).radius)
		{
			children[0]->Insert(newBody, newVolume);
		}
		else
		{
			children[1]->Insert(newBody, newVolume);
		}
	}
}

BoundingSphere::BoundingSphere(const Vector3& center, float radius)
{
	pos = center;
	this->radius = radius;
}

BoundingSphere::BoundingSphere(const BoundingSphere& one, const BoundingSphere& two)
{
	pos = Vector3(
		(one.pos.x + two.pos.x) / 2,
		(one.pos.y + two.pos.y) / 2,
		(one.pos.z + two.pos.z) / 2);

	if(one.radius > two.radius)
	{
		radius = pos.Distance(pos, one.pos) + one.radius;
	}else
	{
		radius = pos.Distance(pos, two.pos) + two.radius;
	}
}

bool BoundingSphere::overlaps(const BoundingSphere* other) const
{
	float distanceSquared = (pos - other->pos).LengthSquared();
	return distanceSquared < (radius + other->radius)*
		(radius + other->radius);
}

Node::Node(Node* node, BoundingSphere volume, Shape* p_body)
{
	parent = node;
	body = p_body;
	bounds = volume;

	children[0] = nullptr;
	children[1] = nullptr;
}

Node::~Node()
{
	// If we don’t have a parent, then we ignore the sibling processing.
	if (parent != nullptr)
	{
		// Find our sibling.
		Node *sibling;
		if (parent->children[0] == this) sibling = parent->children[1];
		else sibling = parent->children[0];
		// Write its data to our parent.
		parent->bounds = sibling->bounds;
		parent->body = sibling->body;
		parent->children[0] = sibling->children[0];
		parent->children[1] = sibling->children[1];
		// Delete the sibling (we blank its parent and
		// children to avoid processing/deleting them).
		sibling->parent = NULL;
		sibling->body = NULL;
		sibling->children[0] = NULL;
		sibling->children[1] = NULL;
		delete sibling;
		// Recalculate the parent’s bounding volume.
		parent->bounds = BoundingSphere(bounds, sibling->bounds);
	}
	// Delete our children (again we remove their parent data so
	// we don’t try to process their siblings as they are deleted).
	if (children[0] != nullptr) {
		children[0]->parent = NULL;
		delete children[0];
	}
	if (children[1] != nullptr) {
		children[1]->parent = NULL;
		delete children[0];
	}
}
