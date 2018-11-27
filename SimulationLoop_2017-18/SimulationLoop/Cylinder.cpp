#include "Cylinder.h"
#include <gl/GLU.h>
#include "TextureLoader.h"


Cylinder::Cylinder(void) :Shape(), m_radius(5), m_height(2){}
Cylinder::~Cylinder()
= default;

void Cylinder::SetRadius(const float p_radius)
{
	m_radius = p_radius;
}

void Cylinder::SetHeight(const float p_height)
{
	m_height = p_height;
}

float Cylinder::GetRadius() const
{
	return m_radius;
}

float Cylinder::GetHeight() const
{
	return m_height;
}

void Cylinder::Render() const
{
	glPushMatrix();
	glTranslatef(GetPos().x, GetPos().y, GetPos().z);
	glColor3d(1, 0, 1); // Here to change Color
	glBindTexture(GL_TEXTURE_2D, GetTexture());               // Select Our Texture
	GLUquadric *quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluCylinder(quadric, m_radius, m_radius, m_height, 20, 20);
	glPopMatrix();
}

float Cylinder::InvertMass()
{
	if (m_mass == 0.0f)
	{
		return 0.0f;
	}
	else
	{
		return 1.0f / m_mass;
	}
}
