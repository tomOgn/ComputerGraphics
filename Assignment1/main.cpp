#include <cmath>
#include <iostream>
#include <stdio.h>     
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GL\glut.h"
#include "types.h"
#include "ufo.h"
using namespace std;

static const GLfloat PI = 3.14159265358979f;
static const int DropsCount = 40;
static const int FlowerSpeed[3][4] = { { 5, 10, 0, 5 }, { 2, 6, 0, 2 }, { 1, 3, 0, 1 } };
static Color BrownWindow(0.54, 0.27, 0.07);
static Color GreenFlower(0.2, 0.6, 0.2);
static Color GreenGrass(0.2, 0.5, 0.0);
static Color BlueFlower(0.2, 0.4, 0.8);
static Color BlueSky(0.5, 0.78, 1.0);
static Color BlueWindow(0.28, 0.82, 0.8);
static Color GrayMoon(0.78, 0.78, 0.78);
static Color GrayMountains(0.6, 0.6, 0.6);
static Color Black(0.0, 0.0, 0.0);
static Color OrangeWall(0.8, 0.5, 0.2);
static Color PurpleUfo(0.6, 0.2, 0.8);
static Color RedFlower(0.6, 0.1, 0.1);
static Color RedRoof(0.8, 0.1, 0.1);
static Color YellowSun(0.9, 0.8, 0.0);
static Color YellosStar(1.0, 1.0, 0.4);
static Color WhiteCloud(0.4, 0.7, 0.9);
static Color * Colors[] = { &BrownWindow, &GreenFlower, &GreenGrass, &BlueFlower, &BlueSky, 
	&BlueWindow, &GrayMoon, &GrayMountains, &OrangeWall, &RedFlower, &RedRoof, &YellowSun, &YellosStar };
static unsigned int SunnyDay = 0, RainyNight = 1, StarryNight = 2, Eclipse = 3;
static unsigned int Time = SunnyDay;
static unsigned int Milliseconds = 0;
static unsigned int WindowWidth = 1024, WindowHeight = 800;
static bool EclipseEnding = false;
static float FlowerRotation = 0.0, EclipseTraslation = 0.0;
Point Drops[DropsCount];
static Ufo ufo(WindowWidth * 0.5, WindowHeight * 0.5, WindowWidth * 0.01);

void Clouds()
{
	glPushMatrix();
	glColor3f(0.4, 0.7, 0.9);

	glLoadIdentity();
	glTranslatef(1, 40.0, -9.0);
	glScalef(2.0, 1.0, 1.0);

	glLoadIdentity();
	glTranslatef(2.0, 40.0, -9.0);
	glScalef(2.0, 1.0, 1.0);
	glutSolidSphere(2.0, 50, 56);

	//glLoadIdentity();
	//glTranslatef(a+7.0,40.0, -9.0);
	//glScalef (2, 1.0, 1.0);
	//glutSolidSphere (2.0,50,56);

	//glLoadIdentity();
	//glTranslatef(a-7.0,40.0, -9.0);
	//glScalef (2, 1.0, 1.0);
	//glutSolidSphere (2.0,50,56);

	//glLoadIdentity();
	//glTranslatef(a+18.0,40.0, -9.0);
	//glScalef (2, 1.0, 1.0);
	//glutSolidSphere (2.0,50,56);

	//glLoadIdentity();
	//glTranslatef(a+25.0,40.0, -9.0);
	//glScalef (2, 1.0, 1.0);
	//glutSolidSphere (2.0,50,56);

	//glLoadIdentity();
	//glTranslatef(a+36.0,40.0, -9.0);
	//glScalef (3.0, 1.0, 1.0);
	//glutSolidSphere (2.0,50,56);

	glPopMatrix();
}


static inline float Min(float a, float b)
{
	return a > b ? b : a;
}

/* Draw a single line with a given dynamic lenght. */
static void DrawLine(float length)
{
	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(0, WindowHeight * length);
	glEnd();
	glTranslatef(0, WindowHeight * length, 0);
}

/* Split the window into two regions (sky and ground) accordingly to a given ratio. */
static void DrawGroundSky(const float ratio)
{
	float y1 = WindowHeight * ratio;

	glBegin(GL_QUADS);
	// Ground
	glColor3f(GreenGrass.R, GreenGrass.G, GreenGrass.B);
	glVertex2f(0.0, 0.0);
	glVertex2f(WindowWidth, 0.0);
	glVertex2f(WindowWidth, y1);
	glVertex2f(0.0, y1);

	// Sky
	if (Time == SunnyDay || Time == Eclipse)
		glColor3f(BlueSky.R, BlueSky.G, BlueSky.B);
	else
		glColor3f(Black.R, Black.G, Black.B);
	glVertex2f(0.0, y1);
	glVertex2f(WindowWidth, y1);
	glVertex2f(WindowWidth, WindowHeight);
	glVertex2f(0.0, WindowHeight);
	glEnd();
}

/* Draw sinusoidal mountains. */
static void DrawMountains()
{
	float y1, y2, y3;

	y1 = WindowHeight * 0.1;
	y2 = WindowHeight * 0.6;
	y3 = WindowHeight * 0.07;

	glBegin(GL_QUADS);
	glColor3f(GrayMountains.R, GrayMountains.G, GrayMountains.B);
	for (unsigned int i = 0, j = 0; i <= WindowWidth; i += 5, ++j)
	{
		glVertex2f(i, y1);
		glVertex2f(i, y2 + y3 * sin(0.1 * j));
		glVertex2f(i + 5.0, y2 + y3 * sin(0.1 * (j + 1)));
		glVertex2f(i + 5.0, y1);
	}
	glEnd();
}

/* Draw a sun. */
static void DrawSun(float x0, float y0, float radius)
{
	float deltaX, deltaY;

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(YellowSun.R, YellowSun.G, YellowSun.B);
	for (int j = 0; j < 360; j += 5)
	{
		deltaX = (float)radius * cos(j);
		deltaY = (float)radius * sin(j);
		glVertex2f(x0 + deltaX, y0 + deltaY);
	}
	glEnd();
}

/* Draw a moon. */
static void DrawMoon(float x0, float y0, float radius)
{
	glBegin(GL_POLYGON);
	glColor3f(GrayMoon.R, GrayMoon.G, GrayMoon.B);
	for (float i = 0; i <= 2 * PI; i += 0.05)
		glVertex2f(radius * cos(i) + x0, radius * sin(i) + y0);
	glEnd();

	x0 += WindowWidth * 0.03125;
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 0);
	for (float i = 0; i <= 2 * PI; i += 0.05)
		glVertex2f(radius * cos(i) + x0, radius * sin(i) + y0);
	glEnd();
}

/* Draw an eclipse. */
static inline void DrawEclipse(float x0, float y0, float radius)
{
	glBegin(GL_POLYGON);
	glColor3f(BlueSky.R, BlueSky.G, BlueSky.B);
	for (float i = 0; i <= 2 * PI; i += 0.05)
		glVertex2f(radius * cos(i) + x0, radius * sin(i) + y0);
	glEnd();
}

void DrawStar(float x, float y, float distance, float angle, float increment, int n_segment)
{
	glPushMatrix();
	glTranslatef(WindowWidth * x, WindowHeight * y, 0); 
	glColor3f(YellosStar.R, YellosStar.G, YellosStar.B);
	glPushMatrix();
	for (int i = 0; i < n_segment; i++)
	{
		glTranslatef(0, WindowHeight * distance, 0);
		glRotatef(45, 0, 0, 1);
		distance += increment;
		glPushMatrix();
		glRotatef(90, 0, 0, 1);
		DrawLine(distance);
		glPushMatrix();
		glRotatef(180, 0, 0, 1);
		DrawLine(distance);
		glPopMatrix();
		glPopMatrix();
	}
	glPopMatrix(); glPopMatrix();
}

static void DrawStars()
{
	DrawStar(0.05, 0.91, 0.0005, 45, 0.0005, 40);
	DrawStar(0.15, 0.75, 0.0005, 45, 0.0005, 40);
	DrawStar(0.23, 0.8, 0.0005, 45, 0.0005, 40);
	DrawStar(0.25, 0.87, 0.0005, 45, 0.0005, 40);
	DrawStar(0.33, 0.9, 0.0005, 45, 0.0005, 40);
	DrawStar(0.37, 0.81, 0.0005, 45, 0.0005, 40);
	DrawStar(0.43, 0.9, 0.0005, 45, 0.0005, 40);
	DrawStar(0.59, 0.81, 0.0005, 45, 0.0005, 40);
	DrawStar(0.64, 0.96, 0.0005, 45, 0.0005, 40);
	DrawStar(0.67, 0.73, 0.0005, 45, 0.0005, 40);
	DrawStar(0.75, 0.88, 0.0005, 45, 0.0005, 40);
	DrawStar(0.9, 0.77, 0.0005, 45, 0.0005, 40);
	DrawStar(0.83, 0.73, 0.0005, 45, 0.0005, 40);
}

/* Draw an house with a window. */
static void DrawHouse(float width, float height, float x0, float y0)
{
	float x1, x2, x3, y1, y2, y3, heightWall = height * 0.6;

	// Wall
	x1 = WindowWidth * x0;
	x2 = WindowWidth * (x0 + width);
	y1 = WindowHeight * y0;
	y2 = WindowHeight * (y0 + heightWall);

	glBegin(GL_QUADS);
	glColor3f(OrangeWall.R, OrangeWall.G, OrangeWall.B);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();

	// Roof
	x1 = WindowWidth * x0;
	x2 = WindowWidth * (x0 + width);
	x3 = WindowWidth * (x0 + width / 2);
	y1 = WindowHeight * (y0 + heightWall);
	y2 = WindowHeight * (y0 + height);

	glBegin(GL_TRIANGLES);
	glColor3f(RedRoof.R, RedRoof.G, RedRoof.B);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x3, y2);
	glEnd();

	// Window
	x1 = WindowWidth * (x0 + width / 2 - width / 6);
	x2 = WindowWidth * (x0 + width / 2 + width / 6);
	y1 = WindowHeight * (y0 + heightWall / 2 + heightWall / 4);
	y2 = WindowHeight * (y0 + heightWall / 2 - heightWall / 4);

	glBegin(GL_POLYGON);
	glColor3f(BlueWindow.R, BlueWindow.G, BlueWindow.B);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
	glLineWidth(2);

	x1 = WindowWidth * (x0 + width / 2 - width / 6);
	x2 = WindowWidth * (x0 + width / 2 + width / 6);
	x3 = WindowWidth * (x0 + width / 2);
	y1 = WindowHeight * (y0 + heightWall / 2 + heightWall / 4);
	y2 = WindowHeight * (y0 + heightWall / 2 - heightWall / 4);
	y3 = WindowHeight * (y0 + heightWall / 2);

	glBegin(GL_LINES);
	glColor3f(BrownWindow.R, BrownWindow.G, BrownWindow.B);
	glVertex2f(x3, y1);
	glVertex2f(x3, y2);
	glVertex2f(x1, y3);
	glVertex2f(x2, y3);
	glEnd();
}

/* Draw a green rotating spiral. */
static void DrawGreenSpiral(float x0, float y0, float length, float angle, float delta, int count, Color color)
{
	glPushMatrix();
	glTranslatef(WindowWidth * x0, WindowHeight * y0, 0);
	glRotatef(FlowerRotation * FlowerSpeed[1][Time], 0, 0, 1);
	glColor3f(color.R, color.G, color.B);
	while (count-- > 0)
	{
		DrawLine(length);
		glRotatef(angle, 0, 0, 1);
		length += delta;

		glPushMatrix();
		glRotatef(angle + 90, 0, 0, 1);
		DrawLine(length);
		glPopMatrix();
	}
	glPopMatrix();
}

/* Draw a blue rotating spiral. */
void DrawBlueSpiral(float x0, float y0, float length, float angle, float delta, int count, Color color)
{
	glTranslatef(WindowWidth * x0, WindowHeight * y0, 0);
	glRotatef(FlowerRotation * FlowerSpeed[2][Time], 0, 0, 1);
	glColor3f(color.R, color.G, color.B);
	glPushMatrix();
	while (count-- > 0)
	{
		glTranslatef(0, WindowHeight * length, 0);
		glRotatef(45, 0, 0, 1);
		length += delta;
		glPushMatrix();
		glRotatef(210, 0, 0, 1);
		DrawLine(length);
		glPushMatrix();
		glRotatef(180, 0, 0, 1);
		DrawLine(length);
		glPopMatrix();
		glPopMatrix();
	}
	glPopMatrix();
}

/* Draw a red rotating spiral. */
static void DrawRedSpiral(float x0, float y0, float length, float angle, float delta, int count, Color color)
{
	glPushMatrix();
	glTranslatef(WindowWidth * x0, WindowHeight * y0, 0);
	glRotatef(FlowerRotation * FlowerSpeed[0][Time], 0, 0, 1);
	glColor3f(color.R, color.G, color.B);
	while (count-- > 0)
	{
		glRotatef(angle, 0, 0, 1);
		DrawLine(length);
		length += delta;
	}
	glPopMatrix();
}

/* Draw a colored thick line. */
static void DrawColoredLine(float x0, float y0, float width, float height, Color color)
{
	glBegin(GL_POLYGON);
	glColor3f(color.R, color.G, color.B);
	glVertex2f(WindowWidth * x0, WindowHeight * (y0 + height));
	glVertex2f(WindowWidth * (x0 + width), WindowHeight * (y0 + height));
	glVertex2f(WindowWidth * (x0 + width), WindowHeight * y0);
	glVertex2f(WindowWidth * x0, WindowHeight * y0);
	glEnd();
}

/* Draw three spiral flowers. */
static void DrawSpiralFlowers()
{
	// Draw a red spiral flower
	DrawColoredLine(0.1, 0.1, 0.004, 0.2, RedFlower);
	DrawRedSpiral(0.1, 0.3, 0.01, 90, 0.005, 20, RedFlower);

	// Draw a green spiral flower
	DrawColoredLine(0.3, 0.1, 0.006, 0.3, GreenFlower);
	DrawGreenSpiral(0.3, 0.4, 0.01, 60, 0.003, 30, GreenFlower);

	// Draw a blue spiral flower
	DrawColoredLine(0.5, 0.1, 0.008, 0.5, BlueFlower);
	DrawBlueSpiral(0.5, 0.6, 0.002, 45, 0.001, 100, BlueFlower);
}

/* Display callback. */
void Display(void)
{
	glPushMatrix();
	glLineWidth(2);
	DrawGroundSky(0.1);
	DrawMountains();
	DrawSpiralFlowers();
	if (Time == StarryNight)
	{
		DrawStars();
		DrawMoon(WindowWidth * 0.9, WindowHeight * 0.9, WindowWidth * 0.05);
	}
	else if (Time == SunnyDay || Time == Eclipse)
		DrawSun(WindowWidth * 0.9, WindowHeight * 0.9, WindowWidth * 0.05);
	if (Time == Eclipse)
	{
		EclipseTraslation -= WindowWidth * 0.0002;
		DrawEclipse(EclipseTraslation, WindowHeight * 0.9, WindowWidth * 0.05);
	}
	DrawHouse(0.25, 0.3, 0.65, 0.1);
	Clouds();
	//DrawUfo(WindowWidth * 0.5, WindowHeight * 0.5);
	glPopMatrix();
	glFlush();
}

/* Reshape callback. */
void Reshape(int Width, int Height)
{
	WindowWidth = Width;
	WindowHeight = Height;
	for (int i = 0; i < DropsCount; ++i)
	{
		Drops[i].X = rand() % Width;
		Drops[i].Y = rand() % Height;
	}
	if (Height == 0) Height = 1;
	glViewport(0, 0, (GLsizei)Width, (GLsizei)Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)Width, 0.0, (GLdouble)Height);
	Display();
}

/* Timer Tick callback. */
void TimerFunction(int value)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Flower rotation
	FlowerRotation += 1;

	// Eclipse event
	if (Time == Eclipse)
	{
		bool EclipseTerminated = true, TotalEclipse = true;
		const float Speed = 0.002;
		for each (Color *color in Colors)
		{
			if (EclipseEnding)
			{
				color->R = (color->R + Speed < color->R2) ? color->R + Speed : color->R2;
				color->G = (color->G + Speed < color->G2) ? color->G + Speed : color->G2;
				color->B = (color->B + Speed < color->B2) ? color->B + Speed : color->B2;
				if (color->R != color->R2 || color->G != color->G2 || color->B != color->B2)
					EclipseTerminated = false;
			}
			else
			{
				color->R = (color->R - Speed > 0) ? color->R - Speed : 0;
				color->G = (color->G - Speed > 0) ? color->G - Speed : 0;
				color->B = (color->B - Speed > 0) ? color->B - Speed : 0;
				if (color->R + color->G + color->B > 0)
					TotalEclipse = false;
			}
		}
		if (!EclipseEnding && TotalEclipse)
			EclipseEnding = true;
		else if (EclipseEnding && EclipseTerminated)
		{
			EclipseEnding = false;
			Time = SunnyDay;
		}
	}

	Display();

	// Rain dropping
	if (Time == RainyNight)
	{
		glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 1.0);
		for (int i = 0; i < DropsCount; i++)
		{
			if (Drops[i].X < 0) Drops[i].X = WindowWidth;
			if (Drops[i].Y < 0) Drops[i].Y = WindowHeight;
			glVertex2f(Drops[i].X, Drops[i].Y);
			glVertex2f(Drops[i].X + 10, Drops[i].Y + 10);
			Drops[i].X -= 15;
			Drops[i].Y -= 15;
		}
		glEnd();
	}
	glFlush();
	glutTimerFunc(25, TimerFunction, 0);
}

/* Menu Click callback */
static void MenuClick(int id)
{
	switch (id)
	{
	case 1:
		Time = SunnyDay;
		break;
	case 2:
		Time = RainyNight;
		break;
	case 3:
		Time = StarryNight;
		break;
	case 4:
		Time = Eclipse;
		EclipseTraslation = WindowWidth;
		break;
	case 5:
		exit(0);
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(500, 200);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow("2D Animation");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutCreateMenu(MenuClick);
	glutAddMenuEntry("A sunny day", 1);
	glutAddMenuEntry("A rainy night", 2);
	glutAddMenuEntry("A starry night", 3);
	glutAddMenuEntry("Eclipse", 4);
	glutAddMenuEntry("Quit", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutTimerFunc(25, TimerFunction, 0);
	glutMainLoop();
	return 0;
}