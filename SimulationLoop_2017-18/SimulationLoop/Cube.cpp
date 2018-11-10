#include "Cube.h"
#include "Rotor3f.h"


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

float Cube::GetLength() const
{
	return m_length;
}

float Cube::GetHeight() const
{
	return m_height;
}

float Cube::GetWidth() const
{
	return m_width;
}

void Cube::CalculatePoints()
{
	m_points[0] = Vector3f( - m_size / 2 - m_length/2,  + m_size / 2 + m_height/2,  + m_size / 2 + m_width/2);
	m_points[1] = Vector3f( + m_size / 2 + m_length/2,  + m_size / 2 + m_height/2,  + m_size / 2 + m_width/2);
	m_points[2] = Vector3f( + m_size / 2 + m_length/2,  + m_size / 2 + m_height/2,  - m_size / 2 - m_width/2);
	m_points[3] = Vector3f( - m_size / 2 - m_length/2,  + m_size / 2 + m_height/2,  - m_size / 2 - m_width/2);
						   							  					
	m_points[4] = Vector3f( - m_size / 2 - m_length/2,  - m_size / 2 - m_height/2,  + m_size / 2 + m_width/2);
	m_points[5] = Vector3f( + m_size / 2 + m_length/2,  - m_size / 2 - m_height/2,  + m_size / 2 + m_width/2);
	m_points[6] = Vector3f( + m_size / 2 + m_length/2,  - m_size / 2 - m_height/2,  - m_size / 2 - m_width/2);
	m_points[7] = Vector3f( - m_size / 2 - m_length/2,  - m_size / 2 - m_height/2,  - m_size / 2 - m_width/2);

	//Rotor3 *rotx = new Rotor3(Vector3f(0, 0, 0), Vector3f(3.14f / 2.0f, 0, 0));
	Rotor3 *rotx = new Rotor3(GetRot().GetX(), Bivector3(1, 0, 0));
	m_points[0] = rotx->rotate(m_points[0]);
	m_points[1] = rotx->rotate(m_points[1]);
	m_points[2] = rotx->rotate(m_points[2]);
	m_points[3] = rotx->rotate(m_points[3]);
				  
	m_points[4] = rotx->rotate(m_points[4]);
	m_points[5] = rotx->rotate(m_points[5]);
	m_points[6] = rotx->rotate(m_points[6]);
	m_points[7] = rotx->rotate(m_points[7]);

	Rotor3 *roty = new Rotor3(GetRot().GetY(), Bivector3(0, 1, 0));
	m_points[0] = roty->rotate(m_points[0]);
	m_points[1] = roty->rotate(m_points[1]);
	m_points[2] = roty->rotate(m_points[2]);
	m_points[3] = roty->rotate(m_points[3]);
				  
	m_points[4] = roty->rotate(m_points[4]);
	m_points[5] = roty->rotate(m_points[5]);
	m_points[6] = roty->rotate(m_points[6]);
	m_points[7] = roty->rotate(m_points[7]);

	Rotor3 *rotz = new Rotor3(GetRot().GetZ(), Bivector3(0, 0, 1));
	m_points[0] = rotz->rotate(m_points[0]);
	m_points[1] = rotz->rotate(m_points[1]);
	m_points[2] = rotz->rotate(m_points[2]);
	m_points[3] = rotz->rotate(m_points[3]);
				  
	m_points[4] = rotz->rotate(m_points[4]);
	m_points[5] = rotz->rotate(m_points[5]);
	m_points[6] = rotz->rotate(m_points[6]);
	m_points[7] = rotz->rotate(m_points[7]);


	m_points[0] = m_points[0] + GetPos();
	m_points[1] = m_points[1] + GetPos();
	m_points[2] = m_points[2] + GetPos();
	m_points[3] = m_points[3] + GetPos();
	
	m_points[4] = m_points[4] + GetPos();
	m_points[5] = m_points[5] + GetPos();
	m_points[6] = m_points[6] + GetPos();
	m_points[7] = m_points[7] + GetPos();


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

	glBegin(GL_LINE_LOOP);
		glColor3d(1, 0, 1);
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

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
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

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
		glVertex3d(m_points[1].GetX(), m_points[1].GetY(), m_points[1].GetZ());
		glVertex3d(m_points[5].GetX(), m_points[5].GetY(), m_points[5].GetZ());
		glVertex3d(m_points[6].GetX(), m_points[6].GetY(), m_points[6].GetZ());
		glVertex3d(m_points[2].GetX(), m_points[2].GetY(), m_points[2].GetZ());
	glEnd();

	//Face 4, pointing towards -x
	glBegin(GL_QUADS);
	glColor3d(1, 11, 1);
		glVertex3d(m_points[0].GetX(), m_points[0].GetY(), m_points[0].GetZ());
		glVertex3d(m_points[4].GetX(), m_points[4].GetY(), m_points[4].GetZ());
		glVertex3d(m_points[7].GetX(), m_points[7].GetY(), m_points[7].GetZ());
		glVertex3d(m_points[3].GetX(), m_points[3].GetY(), m_points[3].GetZ());
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
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

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
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

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
		glVertex3d(m_points[3].GetX(), m_points[3].GetY(), m_points[3].GetZ());
		glVertex3d(m_points[2].GetX(), m_points[2].GetY(), m_points[2].GetZ());
		glVertex3d(m_points[6].GetX(), m_points[6].GetY(), m_points[6].GetZ());
		glVertex3d(m_points[7].GetX(), m_points[7].GetY(), m_points[7].GetZ());
	glEnd();

	
}
