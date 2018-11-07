#include "Cube.h"

Cube::Cube(void) :Shape(), m_size(1), m_width(1), m_height(1), m_length(1)
{
	CalculatePoints();
}

Cube::~Cube()
= default;

void Cube::SetPoints(const Vector3f p_points[8])
{
	for (int i = 0; i < 8; ++i)
	{
		m_points[i] = p_points[i];
	}
}

void Cube::SetSize(const float p_size)
{
	m_size = p_size;
	CalculatePoints();
}

void Cube::SetLength(const float p_length)
{
	m_length = p_length;
	CalculatePoints();
}

void Cube::SetHeight(const float p_height)
{
	m_height = p_height;
	CalculatePoints();
}

void Cube::SetWidth(const float p_width)
{
	m_width = p_width;
	CalculatePoints();
}

void Cube::GetPoints(Vector3f points[8]) const
{

}

void Cube::CalculatePoints()
{
	m_points[0] = Vector3f(GetPos().GetX() - m_size / 2 - m_length/2, GetPos().GetY() + m_size / 2 + m_height/2, GetPos().GetZ() + m_size / 2 + m_width/2);
	m_points[1] = Vector3f(GetPos().GetX() + m_size / 2 + m_length/2, GetPos().GetY() + m_size / 2 + m_height/2, GetPos().GetZ() + m_size / 2 + m_width/2);
	m_points[2] = Vector3f(GetPos().GetX() + m_size / 2 + m_length/2, GetPos().GetY() + m_size / 2 + m_height/2, GetPos().GetZ() - m_size / 2 - m_width/2);
	m_points[3] = Vector3f(GetPos().GetX() - m_size / 2 - m_length/2, GetPos().GetY() + m_size / 2 + m_height/2, GetPos().GetZ() - m_size / 2 - m_width/2);
	
	m_points[4] = Vector3f(GetPos().GetX() - m_size / 2 - m_length/2, GetPos().GetY() - m_size / 2 - m_height/2, GetPos().GetZ() + m_size / 2 + m_width/2);
	m_points[5] = Vector3f(GetPos().GetX() + m_size / 2 + m_length/2, GetPos().GetY() - m_size / 2 - m_height/2, GetPos().GetZ() + m_size / 2 + m_width/2);
	m_points[6] = Vector3f(GetPos().GetX() + m_size / 2 + m_length/2, GetPos().GetY() - m_size / 2 - m_height/2, GetPos().GetZ() - m_size / 2 - m_width/2);
	m_points[7] = Vector3f(GetPos().GetX() - m_size / 2 - m_length/2, GetPos().GetY() - m_size / 2 - m_height/2, GetPos().GetZ() - m_size / 2 - m_width/2);
}

void Cube::Render() const
{
	//Face 1, pointing towards +y
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[0].GetX(), m_points[0].GetY(), m_points[0].GetZ());
		glVertex3d(m_points[1].GetX(), m_points[1].GetY(), m_points[1].GetZ());
		glVertex3d(m_points[2].GetX(), m_points[2].GetY(), m_points[2].GetZ());
		glVertex3d(m_points[3].GetX(), m_points[3].GetY(), m_points[3].GetZ());
	glEnd();

	//Face 2, pointing towards -z
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[3].GetX(), m_points[3].GetY(), m_points[3].GetZ());
		glVertex3d(m_points[2].GetX(), m_points[2].GetY(), m_points[2].GetZ());
		glVertex3d(m_points[6].GetX(), m_points[6].GetY(), m_points[6].GetZ());
		glVertex3d(m_points[7].GetX(), m_points[7].GetY(), m_points[7].GetZ());
	glEnd();

	//Face 3, pointing towards +x
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[1].GetX(), m_points[1].GetY(), m_points[1].GetZ());
		glVertex3d(m_points[5].GetX(), m_points[5].GetY(), m_points[5].GetZ());
		glVertex3d(m_points[6].GetX(), m_points[6].GetY(), m_points[6].GetZ());
		glVertex3d(m_points[2].GetX(), m_points[2].GetY(), m_points[2].GetZ());
	glEnd();

	//Face 4, pointing towards -x
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[0].GetX(), m_points[0].GetY(), m_points[0].GetZ());
		glVertex3d(m_points[4].GetX(), m_points[4].GetY(), m_points[4].GetZ());
		glVertex3d(m_points[7].GetX(), m_points[7].GetY(), m_points[7].GetZ());
		glVertex3d(m_points[3].GetX(), m_points[3].GetY(), m_points[3].GetZ());
	glEnd();

	//Face 5, pointing towards +z
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[0].GetX(), m_points[0].GetY(), m_points[0].GetZ());
		glVertex3d(m_points[4].GetX(), m_points[4].GetY(), m_points[4].GetZ());
		glVertex3d(m_points[5].GetX(), m_points[5].GetY(), m_points[5].GetZ());
		glVertex3d(m_points[1].GetX(), m_points[1].GetY(), m_points[1].GetZ());
	glEnd();

	//Face 6, pointing towards -z
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[3].GetX(), m_points[3].GetY(), m_points[3].GetZ());
		glVertex3d(m_points[2].GetX(), m_points[2].GetY(), m_points[2].GetZ());
		glVertex3d(m_points[6].GetX(), m_points[6].GetY(), m_points[6].GetZ());
		glVertex3d(m_points[7].GetX(), m_points[7].GetY(), m_points[7].GetZ());
	glEnd();
}
