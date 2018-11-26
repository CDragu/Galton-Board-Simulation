#pragma once
#include <Windows.h>
#include <gl\gl.h>  
#include <string>
#include "Rotor3f.h"
#include "d3d11.h"
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;


class Shape
{
public:
	Shape();
	virtual ~Shape();

	void SetName(std::string name);
	void SetPos(float x, float y, float z);
	void SetPos(Vector3 x);
	void SetTexture(GLuint texture);
	void SetRot(float x, float y, float z);
	void SetRot(Vector3 Rot);

	std::string GetName() const;
	Vector3 GetPos() const;
	GLuint GetTexture() const;
	Vector3 GetRot() const;
	Quaternion CreateQuaternion() const;
	Rotor3 CrateRotor() const;

	Matrix CreateTransformMatrix();
	virtual void Render() const;

	std::string m_name;
	int GeometricType; //Sphere = 0, Cube = 1 , Cylinder = 2
	Vector3 m_pos;
	Vector3 m_rot;//in Euler angles TODO: is in radians now
	Vector3 m_scale;

	Matrix transform;
	Rotor3 rotation;

	//Rendering stuff
	int m_objectID;
	GLuint m_texture;
	static int countID;
};

