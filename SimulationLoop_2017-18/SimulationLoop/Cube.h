#pragma once
#include "Shape.h"
class Cube :
	public Shape
{
public:
	Cube(void);
	~Cube(void);

	void SetPoints(const Vector3f p_points[8]);
	void SetSize(const float p_size);
	void SetLength(const float p_length);
	void SetHeight(const float p_height);
	void SetWidth(const float p_width);

	void GetPoints(Vector3f points[8]) const;

	void CalculatePoints();
	void Render() const override;

private:
	Vector3f m_points[8];
	float m_size;
	float m_width;
	float m_height;
	float m_length;
	//Vector3f m_center;
};