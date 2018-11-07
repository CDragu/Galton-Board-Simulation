#pragma once

#include "ContactManifold.h"
#include "Vector3f.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <string>
#include "Shape.h"

class Cylinder : public Shape
{
public:
	Cylinder();
	~Cylinder();

	void SetRadius(float p_radius);
	void SetHeight(float p_height);
	
	float GetRadius() const;
	float GetHeight() const;

	void Render() const override;

private:
	float m_radius;
	float m_height;
};
