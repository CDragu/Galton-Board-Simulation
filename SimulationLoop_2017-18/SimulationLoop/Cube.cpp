#include "Cube.h"
#include "Rotor3f.h"


Cube::Cube(void) :Shape(), m_size(1), m_width(1), m_height(1), m_length(1), m_mass(1), m_friction(0.6f), m_restitution(0.5f)
{
	CalculatePoints();
	DefineInvTensor();
}

Cube::~Cube()
= default;

float Cube::InvertMass()
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

void Cube::DefineInvTensor()
{
	float x, y, z, w = 1;

	float lenght = m_length / 2.0f * m_length / 2.0f;
	float height = m_height / 2.0f * m_height / 2.0f;
	float width =	m_width / 2.0f *  m_width / 2.0f;

	x = (height + width) * m_mass * (1.0f / 12.0f);
	y = (lenght + width) * m_mass * (1.0f / 12.0f);
	z = (lenght + height) * m_mass *(1.0f / 12.0f);

	m_InvTensor = Matrix(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, w).Invert();

}

void Cube::SetPoints(const Vector3 p_points[8])
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

void Cube::GetPoints(Vector3 points[8]) const
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
	m_points[0] = Vector3( - m_size / 2 - m_length/2,  + m_size / 2 + m_height/2,  + m_size / 2 + m_width/2);
	m_points[1] = Vector3( + m_size / 2 + m_length/2,  + m_size / 2 + m_height/2,  + m_size / 2 + m_width/2);
	m_points[2] = Vector3( + m_size / 2 + m_length/2,  + m_size / 2 + m_height/2,  - m_size / 2 - m_width/2);
	m_points[3] = Vector3( - m_size / 2 - m_length/2,  + m_size / 2 + m_height/2,  - m_size / 2 - m_width/2);
						   							  					
	m_points[4] = Vector3( - m_size / 2 - m_length/2,  - m_size / 2 - m_height/2,  + m_size / 2 + m_width/2);
	m_points[5] = Vector3( + m_size / 2 + m_length/2,  - m_size / 2 - m_height/2,  + m_size / 2 + m_width/2);
	m_points[6] = Vector3( + m_size / 2 + m_length/2,  - m_size / 2 - m_height/2,  - m_size / 2 - m_width/2);
	m_points[7] = Vector3( - m_size / 2 - m_length/2,  - m_size / 2 - m_height/2,  - m_size / 2 - m_width/2);

	//Rotor3 *rotx = new Rotor3(Vector3(0, 0, 0), Vector3(3.14f / 2.0f, 0, 0));
	Rotor3 *rotx = new Rotor3(GetRot().x, Bivector3(1, 0, 0));
	m_points[0] = rotx->rotate(m_points[0]);
	m_points[1] = rotx->rotate(m_points[1]);
	m_points[2] = rotx->rotate(m_points[2]);
	m_points[3] = rotx->rotate(m_points[3]);
				  
	m_points[4] = rotx->rotate(m_points[4]);
	m_points[5] = rotx->rotate(m_points[5]);
	m_points[6] = rotx->rotate(m_points[6]);
	m_points[7] = rotx->rotate(m_points[7]);

	Rotor3 *roty = new Rotor3(GetRot().y, Bivector3(0, 1, 0));
	m_points[0] = roty->rotate(m_points[0]);
	m_points[1] = roty->rotate(m_points[1]);
	m_points[2] = roty->rotate(m_points[2]);
	m_points[3] = roty->rotate(m_points[3]);
				  
	m_points[4] = roty->rotate(m_points[4]);
	m_points[5] = roty->rotate(m_points[5]);
	m_points[6] = roty->rotate(m_points[6]);
	m_points[7] = roty->rotate(m_points[7]);

	Rotor3 *rotz = new Rotor3(GetRot().z, Bivector3(0, 0, 1));
	m_points[0] = rotz->rotate(m_points[0]);
	m_points[1] = rotz->rotate(m_points[1]);
	m_points[2] = rotz->rotate(m_points[2]);
	m_points[3] = rotz->rotate(m_points[3]);
				  
	m_points[4] = rotz->rotate(m_points[4]);
	m_points[5] = rotz->rotate(m_points[5]);
	m_points[6] = rotz->rotate(m_points[6]);
	m_points[7] = rotz->rotate(m_points[7]);

	/*m_points[0] = rotation.rotate(m_points[0]);
	m_points[1] = rotation.rotate(m_points[1]);
	m_points[2] = rotation.rotate(m_points[2]);
	m_points[3] = rotation.rotate(m_points[3]);
				 
	m_points[4] = rotation.rotate(m_points[4]);
	m_points[5] = rotation.rotate(m_points[5]);
	m_points[6] = rotation.rotate(m_points[6]);
	m_points[7] = rotation.rotate(m_points[7]);*/

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
		glVertex3d(m_points[0].x, m_points[0].y, m_points[0].z);
		glVertex3d(m_points[1].x, m_points[1].y, m_points[1].z);
		glVertex3d(m_points[2].x, m_points[2].y, m_points[2].z);
		glVertex3d(m_points[3].x, m_points[3].y, m_points[3].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glColor3d(1, 0, 1);
		glVertex3d(m_points[0].x, m_points[0].y, m_points[0].z);
		glVertex3d(m_points[1].x, m_points[1].y, m_points[1].z);
		glVertex3d(m_points[2].x, m_points[2].y, m_points[2].z);
		glVertex3d(m_points[3].x, m_points[3].y, m_points[3].z);
	glEnd();

	//Face 2, pointing towards -z
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[3].x, m_points[3].y, m_points[3].z);
		glVertex3d(m_points[2].x, m_points[2].y, m_points[2].z);
		glVertex3d(m_points[6].x, m_points[6].y, m_points[6].z);
		glVertex3d(m_points[7].x, m_points[7].y, m_points[7].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
		glVertex3d(m_points[3].x, m_points[3].y, m_points[3].z);
		glVertex3d(m_points[2].x, m_points[2].y, m_points[2].z);
		glVertex3d(m_points[6].x, m_points[6].y, m_points[6].z);
		glVertex3d(m_points[7].x, m_points[7].y, m_points[7].z);
	glEnd();

	//Face 3, pointing towards +x
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[1].x, m_points[1].y, m_points[1].z);
		glVertex3d(m_points[5].x, m_points[5].y, m_points[5].z);
		glVertex3d(m_points[6].x, m_points[6].y, m_points[6].z);
		glVertex3d(m_points[2].x, m_points[2].y, m_points[2].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
		glVertex3d(m_points[1].x, m_points[1].y, m_points[1].z);
		glVertex3d(m_points[5].x, m_points[5].y, m_points[5].z);
		glVertex3d(m_points[6].x, m_points[6].y, m_points[6].z);
		glVertex3d(m_points[2].x, m_points[2].y, m_points[2].z);
	glEnd();

	//Face 4, pointing towards -x
	glBegin(GL_QUADS);
	glColor3d(1, 11, 1);
		glVertex3d(m_points[0].x, m_points[0].y, m_points[0].z);
		glVertex3d(m_points[4].x, m_points[4].y, m_points[4].z);
		glVertex3d(m_points[7].x, m_points[7].y, m_points[7].z);
		glVertex3d(m_points[3].x, m_points[3].y, m_points[3].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
		glVertex3d(m_points[0].x, m_points[0].y, m_points[0].z);
		glVertex3d(m_points[4].x, m_points[4].y, m_points[4].z);
		glVertex3d(m_points[7].x, m_points[7].y, m_points[7].z);
		glVertex3d(m_points[3].x, m_points[3].y, m_points[3].z);
	glEnd();

	//Face 5, pointing towards +z
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[0].x, m_points[0].y, m_points[0].z);
		glVertex3d(m_points[4].x, m_points[4].y, m_points[4].z);
		glVertex3d(m_points[5].x, m_points[5].y, m_points[5].z);
		glVertex3d(m_points[1].x, m_points[1].y, m_points[1].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
		glVertex3d(m_points[0].x, m_points[0].y, m_points[0].z);
		glVertex3d(m_points[4].x, m_points[4].y, m_points[4].z);
		glVertex3d(m_points[5].x, m_points[5].y, m_points[5].z);
		glVertex3d(m_points[1].x, m_points[1].y, m_points[1].z);
	glEnd();

	//Face 6, pointing towards -z
	glBegin(GL_QUADS);
	glColor3d(1, 1, 1);
		glVertex3d(m_points[3].x, m_points[3].y, m_points[3].z);
		glVertex3d(m_points[2].x, m_points[2].y, m_points[2].z);
		glVertex3d(m_points[6].x, m_points[6].y, m_points[6].z);
		glVertex3d(m_points[7].x, m_points[7].y, m_points[7].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 1);
		glVertex3d(m_points[3].x, m_points[3].y, m_points[3].z);
		glVertex3d(m_points[2].x, m_points[2].y, m_points[2].z);
		glVertex3d(m_points[6].x, m_points[6].y, m_points[6].z);
		glVertex3d(m_points[7].x, m_points[7].y, m_points[7].z);
	glEnd();

	
}
