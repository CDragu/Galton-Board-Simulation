#pragma once
#include "Shape.h"
class Cube :
	public Shape
{
public:
	Cube(void);
	~Cube(void);

	void SetPoints(const Vector3 p_points[8]);
	void SetSize(const float p_size);
	void SetLength(const float p_length);
	void SetHeight(const float p_height);
	void SetWidth(const float p_width);

	void GetPoints(Vector3 points[8]) const;
	float GetLength() const;
	float GetHeight() const;
	float GetWidth() const;


	void CalculatePoints();
	void Render() const override;


	Vector3 m_points[8];
	float m_size;
	float m_width;
	float m_height;
	float m_length;
};