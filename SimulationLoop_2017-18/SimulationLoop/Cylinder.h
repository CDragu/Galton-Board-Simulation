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

	//void SetName(std::string name);
	//void SetPos(float x, float y, float z);

	//std::string GetName() const;
	
	float GetRadius() const;
	//Vector3f GetPos() const;
	float GetHeight() const;

	void Render() const override;

private:
	//std::string m_name;
	float m_radius;
	float m_height;
	//Vector3f m_pos;
	//int m_objectID;
	//GLuint m_texture;

	//static int countID;
};
