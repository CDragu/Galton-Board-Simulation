#include "Cylinder.h"
#include <gl/GLU.h>
#include "TextureLoader.h"


Cylinder::Cylinder(void) :Shape(), m_radius(5), m_height(2)
{
	//m_objectID = countID;
	//++countID;
	//m_texture = TextureLoader::LoadBMP("checker.bmp");
}


Cylinder::~Cylinder()
= default;

//void Cylinder::SetName(std::string name)
//{
//	m_name = name;
//}
//
//void Cylinder::SetPos(float x, float y, float z)
//{
//	m_pos.Set(x, y, z);
//}

//std::string Cylinder::GetName()const
//{
//	return m_name;
//}

float Cylinder::GetRadius() const
{
	return m_radius;
}


//Vector3f Cylinder::GetPos() const
//{
//	return m_pos;
//}

float Cylinder::GetHeight() const
{
	return m_height;
}

void Cylinder::Render() const
{
	glPushMatrix();
	glTranslatef(GetPos().GetX(), GetPos().GetY(), 0);
	glColor3d(1, 0, 1); // Here to change Color
	glBindTexture(GL_TEXTURE_2D, GetTexture());               // Select Our Texture
	GLUquadric *quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluCylinder(quadric, m_radius, m_radius, m_height, 20, 20);
	glPopMatrix();
}
