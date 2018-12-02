#include "Shape.h"
#include "TextureLoader.h"


int Shape::countID = 0;

Shape::Shape() : m_name("Undefined"), m_pos(0, 0, 0), m_rot(0, 0, 0), m_scale(1, 1, 1)
{
	m_objectID = countID;
	++countID;
	m_texture = TextureLoader::LoadBMP("checker.bmp");
	transform = CreateTransformMatrix();
	rotation = CrateRotor();
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
	transform = CreateTransformMatrix();
	rotation = CrateRotor();
}

void Shape::SetPos(Vector3 x)
{
	m_pos = x;
	transform = CreateTransformMatrix();
	rotation = CrateRotor();
}

void Shape::SetTexture(GLuint texture)
{
	m_texture = texture;
}

void Shape::SetRot(float x, float y, float z)
{
	m_rot = Vector3(x, y, z);
	transform = CreateTransformMatrix();
	rotation = CrateRotor();
}

void Shape::SetRot(Vector3 Rot)
{
	m_rot = Rot;
	transform = CreateTransformMatrix();
	rotation = CrateRotor();
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

Quaternion Shape::CreateQuaternion() const
{
	Quaternion q = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(m_rot.x, m_rot.y, m_rot.z);
	return q;
}

Rotor3 Shape::CrateRotor() const
{
	const auto x = Rotor3(m_rot.x, Bivector3(1, 0, 0));
	const auto y = Rotor3(m_rot.y, Bivector3(0, 1, 0));
	const auto z = Rotor3(m_rot.z, Bivector3(0, 0, 1));

	Rotor3 f = Rotor3();
	f = f.rotate(z).rotate(y).rotate(x);
	
	return f;
}

Matrix Shape::CreateTransformMatrix()
{
	//TRS 
	Matrix p_transform = DirectX::SimpleMath::Matrix::Identity;
	p_transform *= DirectX::SimpleMath::Matrix::CreateRotationX(m_rot.z);
	p_transform *= DirectX::SimpleMath::Matrix::CreateRotationY(m_rot.y);
	p_transform *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_rot.x);
	
	p_transform *= DirectX::SimpleMath::Matrix::CreateTranslation(m_pos);
	
	p_transform *= DirectX::SimpleMath::Matrix::CreateScale(m_scale);


	return p_transform;
}

void Shape::Render() const
{

}
