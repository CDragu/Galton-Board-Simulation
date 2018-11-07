#include "Shape.h"
#include "TextureLoader.h"

int Shape::countID = 0;

Shape::Shape() : m_name("Undefined"), m_pos(0, 0, 0)
{
	m_objectID = countID;
	++countID;
	m_texture = TextureLoader::LoadBMP("checker.bmp");
}


Shape::~Shape()
{
}

void Shape::SetName(std::string name)
{
	m_name = name;
}

void Shape::SetPos(float x, float y, float z)
{
	m_pos.Set(x, y, z);
}

void Shape::SetTexture(GLuint texture)
{
	m_texture = texture;
}

std::string Shape::GetName()const
{
	return m_name;
}


Vector3f Shape::GetPos() const
{
	return m_pos;
}

GLuint Shape::GetTexture() const
{
	return m_texture;
}

void Shape::Render() const
{
	
}
