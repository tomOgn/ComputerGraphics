#include "GL\glut.h"

/* RGB Triple */
class Color
{
public:
	float R, G, B, R2, G2, B2;
	Color(GLfloat r = 0.0f, GLfloat g = 0.0f, GLfloat b = 0.0f)
	{
		R = R2 = r;
		G = G2 = g;
		B = B2 = b;
	}
	void Set(GLfloat r, GLfloat g, GLfloat b)
	{
		R = r;
		G = g;
		B = b;
	}
	void Save()
	{
		R2 = R;
		G2 = G;
		B2 = B;
	}
	void Restore()
	{
		R = R;
		G = G;
		B = B;
	}
};

/* 2D point */
class Point
{
public:
	GLfloat X, Y;
	Point(GLfloat x = 0.0f, GLfloat y = 0.0f)
	{
		X = x;
		Y = y;
	}
};