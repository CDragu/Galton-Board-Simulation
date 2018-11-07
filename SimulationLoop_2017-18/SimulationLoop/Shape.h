#pragma once

#include "ContactManifold.h"
#include "Vector3f.h"
#include <Windows.h>
#include <gl\gl.h>  
#include <string>

class Shape
{
public:
	Shape();
	virtual ~Shape();

	void SetName(std::string name);
	void SetPos(float x, float y, float z);
	void SetTexture(GLuint texture);

	std::string GetName() const;
	Vector3f GetPos() const;
	GLuint GetTexture() const;

	virtual void Render() const;

private:
	std::string m_name;
	Vector3f m_pos;
	int m_objectID;
	GLuint m_texture;

	static int countID;
};

