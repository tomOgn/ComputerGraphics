#include "GL\glut.h"
#include "math.h"

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
	void Clone(Color color)
	{
		this->R = color.R;
		this->G = color.G;
		this->B = color.B;
		this->R2 = color.R2;
		this->G2 = color.G2;
		this->B2 = color.B2;
	}
	void Set(GLfloat r, GLfloat g, GLfloat b)
	{
		R = r;
		G = g;
		B = b;
	}
	void Set(Color color)
	{
		R = color.R;
		G = color.G;
		B = color.B;
	}
	void Save()
	{
		R2 = R;
		G2 = G;
		B2 = B;
	}
	void Restore()
	{
		R = R2;
		G = G2;
		B = B2;
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

/* Koch Snowflake */
class Snowflake
{
public:
	GLfloat X, Y, WindowWidth, WindowHeight;
	Snowflake()
	{
		length = 0.005;
		iteration = 2;
	}
	void Set(GLfloat windowWidth, GLfloat windowHeight)
	{
		this->WindowWidth = windowWidth;
		this->WindowHeight = windowHeight;
	}
	void Draw(GLfloat x, GLfloat y)
	{
		this->X = x;
		this->Y = y;

		glBegin(GL_LINE_LOOP);
		glColor3f(1.0, 1.0, 1.0);
		_Draw(0.0, iteration);
		_Draw(-120.0, iteration);
		_Draw(120.0, iteration);
		glEnd();
	}
private:
	GLfloat length;
	GLint iteration;
	void _Draw(GLfloat angle, GLint iteration)
	{
		GLdouble radian = 0.0174533 * angle;
		GLfloat
			newX = this->X + length * cos(radian) * this->WindowWidth,
			newY = this->Y + length * sin(radian) * this->WindowHeight;

		if (iteration == 0)
		{
			glVertex2f(this->X, this->Y);
			glVertex2f(newX, newY);
			this->X = newX;
			this->Y = newY;
		}
		else
		{
			iteration--;
			glBegin(GL_QUADS);
			_Draw(angle, iteration);
			angle += 60.0;
			_Draw(angle, iteration);
			angle -= 120.0;
			_Draw(angle, iteration);
			angle += 60.0;
			_Draw(angle, iteration);
			glEnd();
		}
	}
};