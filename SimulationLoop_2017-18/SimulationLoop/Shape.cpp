#include "Shape.h"
#include "TextureLoader.h"


int Shape::countID = 0;

Shape::Shape() : m_name("Undefined"), m_pos(0, 0, 0), m_rot(0, 0, 0)
{
	m_objectID = countID;
	++countID;
	m_texture = TextureLoader::LoadBMP("checker.bmp");
	//TRS TODO:this
	transform = DirectX::SimpleMath::Matrix::Identity;
	transform = DirectX::SimpleMath::Matrix::CreateTranslation(m_pos).CreateFromYawPitchRoll(m_rot.x, m_rot.y, m_rot.z).CreateScale(m_scale);

}


Shape::~Shape()
= default;

void Shape::SetName(std::string name)
{
	m_name = name;
}

void Shape::SetPos(float x, float y, float z)
{
	m_pos = Vector3(x, y, z);
	//TRS
	transform = DirectX::SimpleMath::Matrix::Identity;
	transform = DirectX::SimpleMath::Matrix::CreateTranslation(m_pos).CreateFromYawPitchRoll(m_rot.x, m_rot.y, m_rot.z).CreateScale(m_scale);
}

void Shape::SetTexture(GLuint texture)
{
	m_texture = texture;
}

void Shape::SetRot(float x, float y, float z)
{
	m_rot = Vector3(x, y, z);
	//TRS
	transform = DirectX::SimpleMath::Matrix::Identity;
	transform = DirectX::SimpleMath::Matrix::CreateTranslation(m_pos).CreateFromYawPitchRoll(m_rot.x, m_rot.y, m_rot.z).CreateScale(m_scale);
}

std::string Shape::GetName()const
{
	return m_name;
}


Vector3 Shape::GetPos() const
{
	return m_pos;
}

GLuint Shape::GetTexture() const
{
	return m_texture;
}

Vector3 Shape::GetRot() const
{
	return m_rot;
}

void Shape::Render() const
{

}
